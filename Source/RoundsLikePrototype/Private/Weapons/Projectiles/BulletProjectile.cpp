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
#include "Components/FPSAbilitySystemComponent.h"

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

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bAutoActivate = false;

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

	// Binds Sphere Component collision methods to this .cpp scripts equivalent function.
	SphereHitCollision->OnComponentHit.AddDynamic(this, &ABulletProjectile::OnComponentHit);
	SphereOverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &ABulletProjectile::OnComponentBeginOverlapEvent);

	// Activate main visual effect, containing mesh and particles.
	NiagaraComponent->Activate(true);
}

void ABulletProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABulletProjectile, BulletData);
}

//void ABulletProjectile::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

void ABulletProjectile::OnRep_BulletData()
{
	InitializeBulletData(BulletData);
}

void ABulletProjectile::InitializeBulletData(FProjectileSpawnData InBulletData)
{
	FString RoleString = HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
	UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: InitializeBulletData() on [%s]"), *RoleString, *GetName());

	// Store the initial trajectory conditions.
	TrajectoryOrigin = InBulletData.SpawnTransform.GetLocation();
	TrajectoryDirection = InBulletData.SpawnTransform.GetUnitAxis(EAxis::X);

	FVector CameraUp = InBulletData.SpawnTransform.GetUnitAxis(EAxis::Z);

	// Construct an arc direction that is perpendicular to the firing direction
	// while remaining aligned with the player's view "up" direction.
	TrajectoryArcDirection = CameraUp - FVector::DotProduct(CameraUp, TrajectoryDirection) * TrajectoryDirection;
	TrajectoryArcDirection.Normalize();

	const float Verticality = FMath::Abs(FVector::DotProduct(TrajectoryDirection, FVector::UpVector));
	TrajectoryArcStrength = FMath::Pow(1.0f - Verticality, 0.5f);
	TrajectoryArcStrength = FMath::Clamp(1.0f - Verticality, 0.0f, 1.0f) * InBulletData.BulletSpec.BulletArcPitchInfluence;

	// OG Init Code
	BulletData = InBulletData;
	DrawDebugTrajectory();

	ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
	ProjectileMovementComponent->InitialSpeed = 0.0f;
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * InBulletData.BulletSpec.BulletSpeed;
	ProjectileMovementComponent->InitialSpeed = InBulletData.BulletSpec.BulletSpeed;
	ProjectileMovementComponent->MaxSpeed = TNumericLimits<float>::Max();
	ProjectileMovementComponent->ProjectileGravityScale = InBulletData.BulletSpec.BulletGravity;

	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.6f;
	ProjectileMovementComponent->Friction = 0.2f;
	ProjectileMovementComponent->BounceVelocityStopSimulatingThreshold = 5.f;

	ProjectileMovementComponent->bAutoActivate = true;
	ProjectileMovementComponent->Activate();
}

void ABulletProjectile::Tick(float DeltaTime)
{
	//const float NewTime = ProjectileTime + DeltaTime;

	//const FVector NewPosition = CalculatePosition(NewTime);

	//MoveProjectileWithCollision(NewPosition);

	//ProjectileTime = NewTime;
}

FVector ABulletProjectile::CalculateTrajectoryPosition(float Time) const
{
	const FBulletSpec& Spec = BulletData.BulletSpec;

	const float Distance = Spec.BulletSpeed * Time;

	const float ArcDistance =
		FMath::Max(
			Spec.BulletArcDistance,
			1.0f
		);

	const float ArcAlpha = Distance / ArcDistance;

	// Designed spatial arc.
	const float ArcOffset = 4.0f * Spec.BulletArc * TrajectoryArcStrength * ArcAlpha * (1.0f - ArcAlpha);

	// Physical gravity.
	const float GravityAcceleration = GetWorld()->GetGravityZ() * Spec.BulletGravity;

	const FVector GravityOffset = 0.5f * FVector(0.0f, 0.0f, GravityAcceleration) * Time * Time;

	return TrajectoryOrigin + TrajectoryDirection * Distance + TrajectoryArcDirection * ArcOffset + GravityOffset;
}

FVector ABulletProjectile::CalculateTrajectoryVelocity(float Time) const
{
	const FBulletSpec& Spec = BulletData.BulletSpec;

	const float BulletSpeed =
		Spec.BulletSpeed;

	// ------------------------------------------------------------
	// Forward velocity
	// ------------------------------------------------------------

	const FVector ForwardVelocity =
		TrajectoryDirection * BulletSpeed;

	// ------------------------------------------------------------
	// Designed arc velocity
	// ------------------------------------------------------------

	const float Distance =
		BulletSpeed * Time;

	const float ArcDistance =
		FMath::Max(
			Spec.BulletArcDistance,
			1.0f
		);

	const float ArcAlpha = Distance / ArcDistance;

	// Parabolic arc:
	//
	// Position:
	//   Arc = 4 * BulletArc * Alpha * (1 - Alpha)
	//
	// Spatial derivative:
	//   dArc/dDistance =
	//   4 * BulletArc * (1 - 2 * Alpha) / ArcDistance

	const float ArcSlope =
		4.0f
		* Spec.BulletArc
		* TrajectoryArcStrength 
		* (1.0f - 2.0f * ArcAlpha)
		/ ArcDistance;

	const float ArcVelocityMagnitude =
		ArcSlope * BulletSpeed;

	const FVector ArcVelocity =
		TrajectoryArcDirection *
		ArcVelocityMagnitude;

	// ------------------------------------------------------------
	// Physical gravity
	// ------------------------------------------------------------

	const float GravityAcceleration =
		GetWorld()->GetGravityZ() *
		Spec.BulletGravity;

	const FVector GravityVelocity =
		FVector(
			0.0f,
			0.0f,
			GravityAcceleration * Time
		);

	// ------------------------------------------------------------
	// Final velocity
	// ------------------------------------------------------------

	return
		ForwardVelocity
		+ ArcVelocity
		+ GravityVelocity;
}

void ABulletProjectile::MoveProjectileWithCollision(FVector NewPosition)
{





	/*FHitResult Hit;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetInstigator());

	GetWorld()->SweepSingleByChannel(
		Hit,
		LastFramePosition,
		NewPosition,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		CollisionShape,
		Params
	);*/

	// If hit, handle projectile hit. Otherwise, move the actor location.
}

//void ABulletProjectile::InitializeGameplayEffectSpec(FGameplayEffectSpecHandle InEffectSpec)
//{
//	GameplayEffectSpec = InEffectSpec;
//}

#pragma endregion

void ABulletProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BounceCount++;

	if (BounceCount > MaxBounces)
	{
		Destroy();
	}
}

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
		TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpec.Data.Get());
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
		TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpec.Data);
	}
}

#pragma region Helpers
void ABulletProjectile::DrawDebugTrajectory()
{
	const FBulletSpec& Spec = BulletData.BulletSpec;

	const float DebugDistance = Spec.BulletArcDistance * 2.0f;

	const float DebugDuration = DebugDistance / Spec.BulletSpeed;

	const int32 NumSegments = 100;

	FVector PreviousPosition = CalculateTrajectoryPosition(0.0f);

	for (int32 Index = 1; Index <= NumSegments; ++Index)
	{
		const float Alpha = static_cast<float>(Index) / static_cast<float>(NumSegments);

		const float Time = DebugDuration * Alpha;

		const FVector CurrentPosition = CalculateTrajectoryPosition(Time);

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

		const FVector Velocity = CalculateTrajectoryVelocity(Time);

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
#pragma endregion