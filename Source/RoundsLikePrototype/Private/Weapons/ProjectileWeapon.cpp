// Copyrighted Jacob Jones 2026

// Preprocessor Directives
#include "Weapons/ProjectileWeapon.h"
#include "Weapons/Projectiles/BulletProjectile.h"
#include "Weapons/Projectiles/ProjectileUtilities.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"
#include "Weapons/AmmoComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"   
#include "Components/FPSAbilitySystemComponent.h"

AProjectileWeapon::AProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(Mesh);

	AmmoComponent = CreateDefaultSubobject<UAmmoComponent>(TEXT("AmmoComponent"));
}

USkeletalMeshComponent* AProjectileWeapon::GetMesh()
{
	return Mesh;
}

UAmmoComponent* AProjectileWeapon::GetAmmoComponent() const
{
	return AmmoComponent;
}

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();

	Mesh->SetOwnerNoSee(true);
}

void AProjectileWeapon::PrimaryFire(
	const FGameplayAbilitySpecHandle& AbilityHandle, 
	const FGameplayAbilityActivationInfo& ActivationInfo, 
	const FProjectileSpawnData& SpawnData)
{

	SpawnProjectile(AbilityHandle, ActivationInfo, SpawnData);
}

void AProjectileWeapon::SpawnProjectile(
	const FGameplayAbilitySpecHandle& AbilityHandle, 
	const FGameplayAbilityActivationInfo& ActivationInfo, 
	const FProjectileSpawnData& SpawnData)
{
	FString RoleString = GetInstigator()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
	UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: SpawnProjectile() for Weapon [%s]"), *RoleString, IsValid(GetInstigator()) ? *GetInstigator()->GetName() : TEXT("NULL"));

	// Begin Spawning (Deferred)
	ABulletProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABulletProjectile>(
		ProjectileClass,				// Class
		SpawnData.SpawnTransform,		// Transform
		this,							// Owner
		GetInstigator(),				// Instigator
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Projectile) { return; }

	#pragma region Initialize Projectile Variable Members
	Projectile->bPredictedProjectile = !HasAuthority();
	Projectile->SourceAbilityHandle = AbilityHandle;
	Projectile->SourceActivationInfo = ActivationInfo;
	Projectile->ProjectileGameplayEffect = ProjectileGameplayEffect;
	
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();

		if (ASC)
		{
			// Create Data Struct. Stores fire attributes(spread, recoil, etc.) and bullet attributes as FBulletSpec. 
			FProjectileSpawnData BulletData;
			FBulletSpec BulletSpec = UProjectileUtilities::MakeBulletSpec(Cast<UFPSAbilitySystemComponent>(ASC)); // Utility function creates spec using attributes from the owner's ASC GunplayAttributeSet. 
			BulletData.BulletSpec = BulletSpec;
			BulletData.SpawnTransform = SpawnData.SpawnTransform;

			Projectile->InitializeBulletData(BulletData);

			// Create Context for the GameplayEffect that will be delivered. Assign it to the GameplayEffectSpecHandle on Projectile.
			FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
			Context.AddInstigator(GetInstigator(), Projectile);
			Context.AddSourceObject(Projectile);
			Projectile->GameplayEffectSpec = ASC->MakeOutgoingSpec(ProjectileGameplayEffect, 1, Context);
		}
	}
	#pragma endregion

	// Finish Spawning (Deferred)
	Projectile->FinishSpawning(SpawnData.SpawnTransform);

	OnPrimaryFire.Broadcast();
}

#pragma region Utility Functions
bool AProjectileWeapon::CanFire() const
{
	bool bIsProjectileValid = (ProjectileClass && ProjectileGameplayEffect);
	return bIsProjectileValid;
}

TArray<FTransform> AProjectileWeapon::GetMuzzleLocations() const
{
	return TArray<FTransform>();
}
#pragma endregion