// Copyrighted Jacob Jones 2026

#include "Weapons/ProjectileWeapon.h"
#include "Weapons/Projectiles/BulletProjectile.h"
#include "Weapons/Projectiles/ProjectileUtilities.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"

// Constructor
AProjectileWeapon::AProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
}

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AProjectileWeapon::CanFire() const
{
	return true;
}

TArray<FTransform> AProjectileWeapon::GetMuzzleLocations() const
{
	return TArray<FTransform>();
}

void AProjectileWeapon::PrimaryFire(const FGameplayAbilitySpecHandle& AbilityHandle, const FGameplayAbilityActivationInfo& ActivationInfo, const FProjectileSpawnData& SpawnData)
{
	SpawnProjectile(AbilityHandle, ActivationInfo, SpawnData);
}

void AProjectileWeapon::SpawnProjectile(const FGameplayAbilitySpecHandle& AbilityHandle, const FGameplayAbilityActivationInfo& ActivationInfo, const FProjectileSpawnData& SpawnData)
{
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Green, FString::Printf(TEXT("[%s] SpawnProjectile"), *GetInstigator()->GetName()));
	const FTransform& SpawnTransform = SpawnData.SpawnTransform;

	ABulletProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABulletProjectile>(ProjectileClass, SpawnTransform, this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Projectile)
	{
		return;
	}

	Projectile->bPredictedProjectile = !HasAuthority();
	UE_LOG(LogTemp, Warning,
		TEXT("%s spawned projectile. Authority=%d Predicted=%d"),
		*Projectile->GetName(),
		Projectile->HasAuthority(),
		Projectile->bPredictedProjectile);

	Projectile->SourceAbilityHandle = AbilityHandle;
	Projectile->SourceActivationInfo = ActivationInfo;
	Projectile->ProjectileGameplayEffect = ProjectileGameplayEffect;
	
	// Initialize projectile gameplay data

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();

		if (ASC)
		{
			FBulletSpec Spec = UProjectileUtilities::MakeBulletSpec(Cast<UFPSAbilitySystemComponent>(ASC));
			
			FProjectileSpawnData ProjectileData;
			ProjectileData.BulletSpec = Spec;
			ProjectileData.SpawnTransform = SpawnTransform;
			Projectile->InitializeBulletData(ProjectileData);

			FGameplayEffectContextHandle Context = ASC->MakeEffectContext();

			Context.AddSourceObject(this);
			Context.AddInstigator(GetInstigator(), this);

			if (!ProjectileGameplayEffect)
			{
				UE_LOG(LogTemp, Error,
					TEXT("[%s] ProjectileGameplayEffect is NULL. Authority=%d"),
					*GetName(),
					HasAuthority());
				Projectile->FinishSpawning(SpawnTransform);
				return;
			}
			Projectile->GameplayEffectSpec = ASC->MakeOutgoingSpec(ProjectileGameplayEffect, 1, Context);
		}
	}

	Projectile->FinishSpawning(SpawnTransform);
}