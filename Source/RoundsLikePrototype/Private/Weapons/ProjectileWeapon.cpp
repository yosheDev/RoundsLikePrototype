// Copyrighted Jacob Jones 2026

// Preprocessor Directives
#include "Weapons/ProjectileWeapon.h"
#include "Weapons/Projectiles/BulletProjectile.h"
#include "Weapons/Projectiles/ProjectileUtilities.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "Weapons/AmmoComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Enums/WeaponFireType.h"
#include "Weapons/FireData.h"
#include "Components/FPSAbilitySystemComponent.h"

AProjectileWeapon::AProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(Mesh);

	AmmoComponent = CreateDefaultSubobject<UAmmoComponent>(TEXT("AmmoComponent"));
}

void AProjectileWeapon::SyncGunplayAttributes()
{
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();

		if (ASC)
		{
			const UGunplayAttributeSet* GunplayAttributes = ASC->GetSet<UGunplayAttributeSet>();

			if (GunplayAttributes)
			{
				AmmoComponent->SetClipCapacity(FMath::RoundToInt(GunplayAttributes->GetClipCapacity()));
			}
		}
	}
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

	WeaponStream.Initialize(0);
}

void AProjectileWeapon::PrimaryFire(
	const FGameplayAbilitySpecHandle& AbilityHandle, 
	const FGameplayAbilityActivationInfo& ActivationInfo, 
	FProjectileSpawnData& SpawnData,
	const FFireData& FireData)
{
	// Clear any active burst if the player manages to fire again mid-burst
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("Burst Fire Timer Cancelled by PrimaryFire() called."));
	}
	CurrentBurstCount = 0;

	// Cache variables so burst timer loop can access them
	CachedAbilityHandle = AbilityHandle;
	CachedActivationInfo = ActivationInfo;
	CachedSpawnData = SpawnData;
	CachedFireData = FireData;
	
	// Fire initial burst shot.
	ExecuteBurstShot();

	// Schedule remaining burst shots.
	if (CachedFireData.FireBurstAmount > 1 && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			BurstTimerHandle,
			this,
			&AProjectileWeapon::ExecuteBurstShot,
			CachedFireData.FireBurstInterval,
			true
		);
	}
}

void AProjectileWeapon::ExecuteBurstShot()
{
	FString RoleString = GetInstigator()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
	UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: ExecuteBurstShot() for Weapon [%s]"), *RoleString, IsValid(GetInstigator()) ? *GetInstigator()->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: Burst: [%d] Bullets: [%d]"), *RoleString, CachedFireData.FireBurstAmount, CachedFireData.FireBulletAmount);
	if (!GetWorld()) return;

	// Cancel timer if bursts are done already.
	if (CurrentBurstCount >= CachedFireData.FireBurstAmount)
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("Burst Fire Timer Cancelled by Burst being completed."));
		return;
	}


	FRotator ForwardRotation = CachedSpawnData.SpawnTransform.Rotator();

	for (int i = 0; i < CachedFireData.FireBulletAmount; ++i)
	{
		// Reset Rotation var to base forward rotation
		FRotator ModifiedRotation = ForwardRotation;

		#pragma region Get Bullet Angle
		if (CachedFireData.FireSpreadXAngle > 0.0f || CachedFireData.FireSpreadZAngle > 0.0f)
		{
			// Pick a random angle around the unit circle
			float RandomAngle = WeaponStream.FRandRange(0.0f, 2.0f * PI);

			// Distribute uniformly across the radius
			float RandomRadius = FMath::Sqrt(WeaponStream.FRand());

			// Calculate elliptical offsets (X-Angle maps to Yaw, Z-Angle maps to Pitch)
			float YawOffset = RandomRadius * CachedFireData.FireSpreadXAngle * FMath::Cos(RandomAngle);
			float PitchOffset = RandomRadius * CachedFireData.FireSpreadZAngle * FMath::Sin(RandomAngle);

			ModifiedRotation.Yaw += YawOffset;
			ModifiedRotation.Pitch += PitchOffset;
		}

		FProjectileSpawnData NewSpawnData = CachedSpawnData;
		NewSpawnData.SpawnTransform.SetRotation(ModifiedRotation.Quaternion());
		#pragma endregion

		SpawnProjectile(CachedAbilityHandle, CachedActivationInfo, NewSpawnData);
	}

	CurrentBurstCount++;

	if (CurrentBurstCount >= CachedFireData.FireBurstAmount)
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("Burst Fire Timer Cancelled by Burst being completed."));
	}
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

			AmmoComponent->TryConsumeAmmo();
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