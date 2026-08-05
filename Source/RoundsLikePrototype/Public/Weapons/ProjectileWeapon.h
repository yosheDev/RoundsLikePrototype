// Copyrighted Jacob Jones 2026

#pragma once

// Preprocessor Directives
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/IWeapon.h"
#include "ProjectileWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrimaryFireDelegate);

UCLASS()
class ROUNDSLIKEPROTOTYPE_API AProjectileWeapon : public AActor, public IWeapon
{
	GENERATED_BODY()
	
	/** Weapon Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

protected:
	
	/** Class of projectile this weapon fires. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABulletProjectile> ProjectileClass;

	/** Class of GAS Gameplay Effect the projectile delivers to targets. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> ProjectileGameplayEffect;

	UPROPERTY()
	bool bTriggerringPrimary = false;

public:	
	
	AProjectileWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	USkeletalMeshComponent* GetMesh();

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
		const FProjectileSpawnData& SpawnData);

protected:
	
	virtual void BeginPlay() override;

	/** Handles firing a projectile. */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SpawnProjectile(
		const FGameplayAbilitySpecHandle& AbilityHandle, 
		const FGameplayAbilityActivationInfo& ActivationInfo, 
		const FProjectileSpawnData& SpawnData);

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPrimaryFireDelegate OnPrimaryFire;
};
