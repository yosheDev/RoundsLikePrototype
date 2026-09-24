// Copyrighted Jacob Jones 2026

#pragma once

// Preprocessor Directives
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/IWeapon.h"
#include "Weapons/FireData.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"
#include "GameplayAbilitySpecHandle.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "ProjectileWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrimaryFireDelegate);

UCLASS()
class ROUNDSLIKEPROTOTYPE_API AProjectileWeapon : public AActor, public IWeapon
{
	GENERATED_BODY()
	
public:
	/** Weapon Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

protected:
	
	/** Ammo Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAmmoComponent> AmmoComponent;

	/** Class of projectile this weapon fires. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABulletProjectile> ProjectileClass;

	/** Class of GAS Gameplay Effect the projectile delivers to targets. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> ProjectileGameplayEffect;

	UPROPERTY()
	bool bTriggerringPrimary = false;

	FRandomStream WeaponStream;

public:	
	
	AProjectileWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	USkeletalMeshComponent* GetMesh();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	UAmmoComponent* GetAmmoComponent() const;

	/** Is this weapon currently able to fire a projectile? */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool CanFire() const;

	// Currently unused.
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual TArray<FTransform> GetMuzzleLocations() const;

	/** Handles shooting of projectiles accounting for weapon stats. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void PrimaryFire(
		const FGameplayAbilitySpecHandle& AbilityHandle, 
		const FGameplayAbilityActivationInfo& ActivationInfo, 
		FProjectileSpawnData& SpawnData,
		const FFireData& FireData);

	// Call this after ASC is initialized with correct data. Syncs Attributes with any restored or modified GunplayAttributes data.
	void SyncGunplayAttributes();

protected:
	
	virtual void BeginPlay() override;

	/** Handles firing a projectile. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SpawnProjectile(
		const FGameplayAbilitySpecHandle& AbilityHandle, 
		const FGameplayAbilityActivationInfo& ActivationInfo, 
		const FProjectileSpawnData& SpawnData);

	#pragma region Burst Logic
	// Tracks how many burst steps we have fired so far
	int32 CurrentBurstCount = 0;

	// Handle to clear or manage the burst timer safely
	FTimerHandle BurstTimerHandle;

	// Cached copies needed for the timer loop
	FGameplayAbilitySpecHandle CachedAbilityHandle;
	FGameplayAbilityActivationInfo CachedActivationInfo;
	FProjectileSpawnData CachedSpawnData;
	FFireData CachedFireData;

	// The internal function that the timer will call repeatedly
	void ExecuteBurstShot();
	#pragma endregion

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPrimaryFireDelegate OnPrimaryFire;
};
