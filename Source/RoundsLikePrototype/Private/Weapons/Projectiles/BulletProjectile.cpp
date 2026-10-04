// Copyright Jacob Jones 2026


// Preprocessor Directives
#include "Weapons/Projectiles/BulletProjectile.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/Projectiles/BulletSpec.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "HAL/IConsoleManager.h"
#include "Components/FPSAbilitySystemComponent.h"

// CVAR for enabling/disabling trajectory traces.
static TAutoConsoleVariable<bool> CVarBulletTraces(
	TEXT("Bullet.Trajectory"),						// Name typed in the console
	false,											// Default value
	TEXT("Enables trajectory traces for bullets."), // Help/tooltip text
	ECVF_Default									// Flags (e.g., ECVF_Cheat for cheat-only)
);


#pragma region Initialization
ABulletProjectile::ABulletProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Client-only prediction projectiles do not replicate. Only SERVER projectiles replicate.
	if (!HasAuthority())
	{
		SetReplicates(false);
	}
	else
	{
		SetReplicates(true);
	}
	bAlwaysRelevant = true;

#pragma region Create Components
	SphereHitCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereHitCollision"));
	SphereHitCollision->SetNotifyRigidBodyCollision(true);
	SetRootComponent(SphereHitCollision);

	SphereOverlapCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereOverlapCollision"));
	SphereOverlapCollision->SetupAttachment(RootComponent);
	SphereOverlapCollision->SetGenerateOverlapEvents(true);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffectComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->bAutoActivate = false;
#pragma endregion
}

void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GetInstigator()))
	{
		Destroy();
		return;
	}

	// If I am the client and this projectile is coming from the character I control, but it is not one of my predicted projectiles.
	if (!HasAuthority() && GetInstigator()->IsLocallyControlled() && !bPredictedProjectile)
	{
		// Hide and deactivate collision.
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		Destroy();
		return;
	}

	SphereOverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &ABulletProjectile::OnComponentBeginOverlapEvent);

	// Activate main visual effect, containing mesh and particles.
	NiagaraComponent->Activate(true);
}

void ABulletProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABulletProjectile, BulletData);
}

void ABulletProjectile::OnRep_BulletData()
{
	InitializeBulletData(BulletData);
}

void ABulletProjectile::InitializeBulletData(FProjectileSpawnData InBulletData)
{
	FString RoleString = HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
	UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: InitializeBulletData() on [%s]"), *RoleString, *GetName());

	BulletData = InBulletData;

	// Set bullet size. Only increase the overlap col and niagara size. This way, bullets that are too big don't destroy on environment too easily.
	//SetActorScale3D(GetActorScale3D() * InBulletData.BulletSpec.BulletSize);
	SphereOverlapCollision->SetRelativeScale3D(SphereOverlapCollision->GetRelativeScale3D() * InBulletData.BulletSpec.BulletSize);
	NiagaraComponent->SetFloatParameter(FName(TEXT("ActorScale")), NiagaraComponent->GetRelativeScale3D().X * InBulletData.BulletSpec.BulletSize);

#pragma region Initialize Trajectory
	// Store the initial trajectory conditions.
	TrajectoryOrigin = InBulletData.SpawnTransform.GetLocation();
	TrajectoryDirection = InBulletData.SpawnTransform.GetUnitAxis(EAxis::X);
	CurrentTrajectorySpeed = InBulletData.BulletSpec.BulletSpeed;
	FVector CameraUp = InBulletData.SpawnTransform.GetUnitAxis(EAxis::Z);

	// Construct an arc direction that is perpendicular to the firing direction while remaining aligned with the player's view up direction.
	TrajectoryArcDirection = CameraUp - FVector::DotProduct(CameraUp, TrajectoryDirection) * TrajectoryDirection;
	TrajectoryArcDirection.Normalize();

	// This affects the arc based on pitch of the player aim.
	const float Verticality = FMath::Abs(FVector::DotProduct(TrajectoryDirection, FVector::UpVector));
	TrajectoryArcStrength = FMath::Pow(1.0f - Verticality, 0.5f);
	TrajectoryArcStrength = FMath::Clamp(TrajectoryArcStrength, 0.0f, 1.0f) * InBulletData.BulletSpec.BulletArcPitchInfluence;

	ArcEndTime = FMath::Max(InBulletData.BulletSpec.BulletArcDistance, 1.0f) / CurrentTrajectorySpeed;

	ProjectileTime = 0.0f;
	PreviousTrajectoryPosition = CalculateTrajectoryPosition(0.0f);
	bHasEnteredNaturalTrajectory = false;
	NaturalTrajectoryOrigin = FVector::ZeroVector;
	NaturalTrajectoryInitialVelocity = FVector::ZeroVector;
	NaturalTrajectoryTime = 0.0f;
	ArcEndTime = FMath::Max(InBulletData.BulletSpec.BulletArcDistance, 1.0f) / CurrentTrajectorySpeed;

	if (CVarBulletTraces.GetValueOnGameThread())
	{
		DrawDebugTrajectory();
	}
#pragma endregion
}
#pragma endregion


#pragma region Bullet Behavior
void ABulletProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasBounced)
	{
		#pragma region Natural Physics Projectile

		const FVector StartPosition = GetActorLocation();
		PostBounceVelocity += FVector(0.0f, 0.0f, GetWorld()->GetGravityZ() * BulletData.BulletSpec.BulletGravity) * DeltaTime;
		const FVector EndPosition = StartPosition + PostBounceVelocity * DeltaTime;

		MoveProjectileWithCollision(StartPosition, EndPosition);
		return;
		#pragma endregion
	}

	#pragma region Custom Arc Time Function Projectile Physics

	const float NewTime = ProjectileTime + DeltaTime;

	if (!bHasEnteredNaturalTrajectory && NewTime >= ArcEndTime)
	{
		const float RemainingTime = NewTime - ArcEndTime;

		const FVector ArcEndPosition = CalculateTrajectoryPosition(ArcEndTime);

		const bool bHitArcEnd = MoveProjectileWithCollision(PreviousTrajectoryPosition, ArcEndPosition);

		if (bHitArcEnd)
		{
			PreviousTrajectoryPosition = GetActorLocation();
			return;
		}

		TransitionToNaturalTrajectory();

		NaturalTrajectoryTime = RemainingTime;

		const FVector NaturalPosition = CalculateTrajectoryPosition(NaturalTrajectoryTime);

		const bool bHitNatural = MoveProjectileWithCollision(ArcEndPosition, NaturalPosition);

		if (!bHitNatural)
		{
			PreviousTrajectoryPosition = NaturalPosition;
			ProjectileTime = NewTime;
		}
		else
		{
			PreviousTrajectoryPosition = GetActorLocation();
		}

		return;
	}

	const FVector NewPosition = CalculateTrajectoryPosition(bHasEnteredNaturalTrajectory ? NaturalTrajectoryTime + DeltaTime : NewTime);

	const bool bHit = MoveProjectileWithCollision(PreviousTrajectoryPosition, NewPosition);

	if (!bHit)
	{
		PreviousTrajectoryPosition = NewPosition;

		if (bHasEnteredNaturalTrajectory)
		{
			NaturalTrajectoryTime += DeltaTime;
		}
		else
		{
			ProjectileTime = NewTime;
		}
	}
	else
	{
		PreviousTrajectoryPosition = GetActorLocation();
	}
	#pragma endregion
}


FVector ABulletProjectile::CalculateTrajectoryPosition(float Time)
{
	const FBulletSpec& Spec = BulletData.BulletSpec;

	if (bHasEnteredNaturalTrajectory)
	{
		constexpr float GravityReferenceDistance = 1000.0f;
		constexpr float GravityReferenceDrop = 100.0f;

		const float GravityAcceleration = -2.0f * GravityReferenceDrop * Spec.BulletGravity * FMath::Square(CurrentTrajectorySpeed) / FMath::Square(GravityReferenceDistance);

		const FVector GravityVector = FVector(0.0f, 0.0f, GravityAcceleration);

		return NaturalTrajectoryOrigin + NaturalTrajectoryInitialVelocity * Time + 0.5f * GravityVector * Time * Time;
	}

	const float Distance = CurrentTrajectorySpeed * Time;

	FVector Position = TrajectoryOrigin + (TrajectoryDirection * Distance);

	#pragma region Designed Arc
	const float ArcDistance = FMath::Max(Spec.BulletArcDistance, 1.0f);
	const float ArcAlpha = FMath::Clamp(Distance / ArcDistance, 0.0f, 1.0f);
	const float ArcOffset = 4.0f * Spec.BulletArc * TrajectoryArcStrength * ArcAlpha * (1.0f - ArcAlpha);

	Position += TrajectoryArcDirection * ArcOffset;
	#pragma endregion

	// Distance-based gravity (Gravity continues after bouncing.)

	constexpr float GravityReferenceDistance = 1000.0f;
	constexpr float GravityReferenceDrop = 100.0f;

	const float GravityAlpha = Distance / GravityReferenceDistance;
	const float GravityDrop = GravityReferenceDrop * Spec.BulletGravity * GravityAlpha * GravityAlpha;

	Position += FVector(0.0f, 0.0f, -GravityDrop);
	return Position;
}


FVector ABulletProjectile::CalculateTrajectoryVelocity(float Time)
{
	const FBulletSpec& Spec = BulletData.BulletSpec;

	const float BulletSpeed = CurrentTrajectorySpeed;

	const float Distance = BulletSpeed * Time;

	const FVector ForwardVelocity = TrajectoryDirection * BulletSpeed;

	FVector ArcVelocity = FVector::ZeroVector;

	if (!bHasEnteredNaturalTrajectory)
	{
		#pragma region Designed Arc Velocity
		const float ArcDistance = FMath::Max(Spec.BulletArcDistance, 1.0f);
		const float ArcAlpha = FMath::Clamp(Distance / ArcDistance, 0.0f, 1.0f);
		const float ArcSlope = 4.0f * Spec.BulletArc * TrajectoryArcStrength * (1.0f - 2.0f * ArcAlpha) / ArcDistance;
		ArcVelocity = TrajectoryArcDirection * ArcSlope * BulletSpeed;
		#pragma endregion
	}

	// Distance-based gravity velocity (Gravity continues after bouncing.)
	constexpr float GravityReferenceDistance = 1000.0f;
	constexpr float GravityReferenceDrop = 100.0f;

	const float GravitySlope = 2.0f * GravityReferenceDrop * Spec.BulletGravity * Distance / (GravityReferenceDistance * GravityReferenceDistance);
	const FVector GravityVelocity = FVector(0.0f, 0.0f, -GravitySlope * BulletSpeed);

	return ForwardVelocity + ArcVelocity + GravityVelocity;
}


void ABulletProjectile::TransitionToNaturalTrajectory()
{
	if (bHasEnteredNaturalTrajectory)
	{
		return;
	}

	NaturalTrajectoryOrigin = CalculateTrajectoryPosition(ArcEndTime);
	NaturalTrajectoryInitialVelocity = CalculateTrajectoryVelocity(ArcEndTime);
	NaturalTrajectoryTime = 0.0f;

	bHasEnteredNaturalTrajectory = true;
}


bool ABulletProjectile::MoveProjectileWithCollision(const FVector& StartPosition, const FVector& EndPosition)
{
	if (!GetWorld())
	{
		return false;
	}

	FHitResult Hit;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BulletProjectileSweep), false, this);

	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetInstigator());

	const FVector SweepDelta = EndPosition - StartPosition;

	if (SweepDelta.IsNearlyZero())
	{
		SetActorLocation(EndPosition);
		return false;
	}

	// Sweep using params from SphereHitCollision (radius, channels, etc.)
	const float CollisionRadius = SphereHitCollision->GetScaledSphereRadius();

	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(CollisionRadius);

	const bool bHit =
		GetWorld()->SweepSingleByChannel(
			Hit,
			StartPosition,
			EndPosition,
			FQuat::Identity,
			SphereHitCollision->GetCollisionObjectType(),
			CollisionShape,
			QueryParams);

	// No collision.
	if (!bHit)
	{
		BulletTravelDistance += FVector::Distance(StartPosition, EndPosition);

		SetActorLocation(EndPosition);
		return false;
	}

	// Collision.
	const float SegmentDistance = FVector::Distance(StartPosition, Hit.Location);
	BulletTravelDistance += SegmentDistance;

	SetActorLocation(Hit.Location);
	BounceProjectile(Hit);
	return true;
}


void ABulletProjectile::BounceProjectile(const FHitResult& Hit)
{
	BounceCount++;

	const FBulletSpec& Spec = BulletData.BulletSpec;

	if (BounceCount > Spec.MaxBounces)
	{
		Destroy();
		return;
	}

	#pragma region Reflect Velocity And Get New Velocity

	FVector IncomingVelocity = FVector::Zero();

	if (!bHasBounced)
	{
		IncomingVelocity = CalculateTrajectoryVelocity(ProjectileTime);
	}
	else
	{
		IncomingVelocity = PostBounceVelocity;
	}

	if (IncomingVelocity.IsNearlyZero())
	{
		Destroy();
		return;
	}

	const FVector SurfaceNormal = Hit.ImpactNormal.GetSafeNormal();

	PostBounceVelocity = FMath::GetReflectionVector(IncomingVelocity, SurfaceNormal);
	const float BounceRetention = FMath::Clamp((BounceCount > 1) ? (1.0 - (BounceCount * 0.02f)) : Spec.BulletBounceVelocityRetention, 0.0f, 1.0f);

	PostBounceVelocity *= BounceRetention;

	#pragma endregion

	// Transition physics mode into natural ballistics simulation.
	bHasBounced = true;

	// Move slightly away from the surface.
	SetActorLocation(Hit.Location + SurfaceNormal * 1.0f);

	PreviousTrajectoryPosition = GetActorLocation();

	ProjectileTime = 0.0f;
}

#pragma endregion

#pragma region Damage
void ABulletProjectile::OnComponentBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If OtherActor is a valid target, excluded this projectiles Instigator. (Certain bullets may be able to affect the instigator??)
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator()) { return; }

	if (!HasAuthority() && bPredictedProjectile)
	{
		PredictDamage(OtherActor);
		Destroy();
		return;
	}

	if (HasAuthority())
	{
		ApplyDamage(OtherActor);
		Destroy();
		return;
	}
}


void ABulletProjectile::PredictDamage(AActor* Target)
{
	// Get ASC Properly
	IAbilitySystemInterface* TargetInterface = Cast<IAbilitySystemInterface>(Target);
	if (!TargetInterface) { return; }
	UAbilitySystemComponent* TargetASC = TargetInterface->GetAbilitySystemComponent();
	if (!TargetASC) { return; }
	UAbilitySystemComponent* SourceASC = Cast<IAbilitySystemInterface>(GetInstigator())->GetAbilitySystemComponent();
	if (!SourceASC) { return; }

	if (GameplayEffectSpec.IsValid())
	{
		FString RoleString = HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
		UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: PredictDamage() from [%s]"), *RoleString, *GetName());

		FGameplayEffectSpec* Spec = GameplayEffectSpec.Data.Get();

		const FGameplayTag TravelDistanceTag = FGameplayTag::RequestGameplayTag(TEXT("Data.BulletTravelDistance"));
		Spec->SetSetByCallerMagnitude(TravelDistanceTag, GetBulletTravelDistance());

		TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}


void ABulletProjectile::ApplyDamage(AActor* Target)
{
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Target);
	if (!ASCInterface) { return; }

	UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();

	if (TargetASC && GameplayEffectSpec.IsValid())
	{
		FString RoleString = HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
		UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: ApplyDamage() from [%s]"), *RoleString, *GetName());

		FGameplayEffectSpec* Spec = GameplayEffectSpec.Data.Get();

		const FGameplayTag TravelDistanceTag = FGameplayTag::RequestGameplayTag(TEXT("Data.BulletTravelDistance"));
		Spec->SetSetByCallerMagnitude(TravelDistanceTag, GetBulletTravelDistance());

		TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}
#pragma endregion


#pragma region Helpers
void ABulletProjectile::DrawDebugTrajectory()
{
	const FBulletSpec& Spec = BulletData.BulletSpec;

	const float DebugDistance = Spec.BulletArcDistance * 2.0f;

	const float DebugDuration = DebugDistance / Spec.BulletSpeed;

	const float ArcDistance = FMath::Max(Spec.BulletArcDistance, 1.0f);
	const float _ArcEndTime = ArcDistance / CurrentTrajectorySpeed;

	const int32 NumSegments = 100;

	// Calculate the exact state at the end of the designed arc.
	const FVector ArcEndPosition = CalculateTrajectoryPosition(_ArcEndTime);

	const FVector ArcEndVelocity = CalculateTrajectoryVelocity(_ArcEndTime);

	constexpr float GravityReferenceDistance = 1000.0f;
	constexpr float GravityReferenceDrop = 100.0f;

	const float GravityAcceleration = -2.0f * GravityReferenceDrop * Spec.BulletGravity * FMath::Square(CurrentTrajectorySpeed) / FMath::Square(GravityReferenceDistance);

	const FVector GravityVector = FVector(0.0f, 0.0f, GravityAcceleration);

	FVector PreviousPosition = CalculateTrajectoryPosition(0.0f);

	for (int32 Index = 1; Index <= NumSegments; ++Index)
	{
		const float Alpha = static_cast<float>(Index) / static_cast<float>(NumSegments);

		const float Time = DebugDuration * Alpha;

		FVector CurrentPosition;

		if (Time <= _ArcEndTime)
		{
			// Designed arc phase.
			CurrentPosition = CalculateTrajectoryPosition(Time);
		}
		else
		{
			// Natural trajectory phase.
			const float NaturalTime = Time - _ArcEndTime;

			CurrentPosition =
				ArcEndPosition +
				ArcEndVelocity * NaturalTime +
				0.5f * GravityVector * NaturalTime * NaturalTime;
		}

		DrawDebugLine(
			GetWorld(),
			PreviousPosition,
			CurrentPosition,
			FColor::Green,
			false,
			10.0f,
			0,
			2.0f
		);

		FVector Velocity;

		if (Time <= _ArcEndTime)
		{
			Velocity = CalculateTrajectoryVelocity(Time);
		}
		else
		{
			const float NaturalTime = Time - _ArcEndTime;

			Velocity = ArcEndVelocity + GravityVector * NaturalTime;
		}

		DrawDebugLine(
			GetWorld(),
			CurrentPosition,
			CurrentPosition + Velocity.GetSafeNormal() * 100.0f,
			FColor::Yellow,
			false,
			10.0f,
			0,
			1.0f
		);

		PreviousPosition = CurrentPosition;
	}
}


float ABulletProjectile::GetBulletTravelDistance() const
{
	return BulletTravelDistance;
}
#pragma endregion