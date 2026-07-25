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
	PrimaryActorTick.bCanEverTick = false;

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

	BulletData = InBulletData;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
	ProjectileMovementComponent->InitialSpeed = 0.0f;
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * InBulletData.BulletSpec.BulletSpeed;
	ProjectileMovementComponent->InitialSpeed = InBulletData.BulletSpec.BulletSpeed;
	ProjectileMovementComponent->MaxSpeed = TNumericLimits<float>::Max();
	ProjectileMovementComponent->ProjectileGravityScale = InBulletData.BulletSpec.BulletGravity;

	ProjectileMovementComponent->bAutoActivate = true;
	ProjectileMovementComponent->Activate();
}

//void ABulletProjectile::InitializeGameplayEffectSpec(FGameplayEffectSpecHandle InEffectSpec)
//{
//	GameplayEffectSpec = InEffectSpec;
//}

#pragma endregion

void ABulletProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
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