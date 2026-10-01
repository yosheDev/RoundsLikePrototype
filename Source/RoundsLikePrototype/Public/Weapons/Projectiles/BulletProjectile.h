// Copyright Jacob Jones 2026

#pragma once

// Preprocessor Directives
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "Weapons/Projectiles/BulletSpec.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "BulletProjectile.generated.h"

// Forward Declarations
class UNiagaraSystem;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API ABulletProjectile : public AActor
{
	GENERATED_BODY()
	
#pragma region Components
protected:
	/** The collision used for Hit events. Mostly for environmental collisions. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> SphereHitCollision;

	/** The collision used for Overlap events. Mostly used for hitbox interaction. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> SphereOverlapCollision;

	/** The primary Visual Effect of the projectile. Acts as the main visual body. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UNiagaraComponent> NiagaraComponent;

#pragma endregion

public:	

	ABulletProjectile();

	// Initializes attributes relevant to local projectile(movement, size, traits)
	void InitializeBulletData(FProjectileSpawnData SpawnData);

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	#pragma region Calculations
	UFUNCTION()
	FVector CalculateTrajectoryPosition(float Time) const;
	FVector CalculateTrajectoryVelocity(float Time) const;

	UFUNCTION()
	bool MoveProjectileWithCollision(const FVector& PreviousPosition, const FVector& NewPosition);

	UFUNCTION()
	void DrawDebugTrajectory();

	FVector TrajectoryOrigin;
	FVector TrajectoryDirection;
	FVector TrajectoryArcDirection;
	float TrajectoryArcStrength = 1.0f;

	UPROPERTY()
	FVector CurrentVelocity;

	UPROPERTY()
	float CurrentTrajectorySpeed = 0.0f;

	UPROPERTY()
	float ProjectileTime = 0.0f;

	UPROPERTY()
	FVector PreviousTrajectoryPosition = FVector::ZeroVector;

	UFUNCTION()
	void BounceProjectile(const FHitResult& Hit);

	// Records if has bounced, as after bouncing trajectory becomes more physics like with no arcs or anything.
	UPROPERTY()
	bool bHasBounced = false;

	UPROPERTY()
	FVector PostBounceVelocity = FVector::ZeroVector;

	UPROPERTY()
	uint8 BounceCount = 0;
	#pragma endregion


	UFUNCTION()
	void OnComponentBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

	/** Triggers when a CLIENT-only predicted projectile hits a target. Affects damage display, but will be corrected when the server replicates health. */
	UFUNCTION()
	void PredictDamage(AActor* Target);

	/** Triggers when the projectile hits a target on the SERVER. Authoratative damage event. */
	UFUNCTION()
	void ApplyDamage(AActor* Target);

protected:

	/** Bullet attributes from GunplayAttributeSet. */
	UPROPERTY(ReplicatedUsing = OnRep_BulletData)
	FProjectileSpawnData BulletData;

	UFUNCTION()
	void OnRep_BulletData();

public:

	//	TODO: When I was trying to make stuff work, I ended up needing the UGameplayEffect here and not just the handle...Why? How bad is this? Can I avoid it?

	/** Spec Handle for the Gameplay Effect that this projectile will deliver. Also contains context towards the instigator(pawn) and source object(weapon). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayEffectSpecHandle GameplayEffectSpec;

	/** Literal class of the Gameplay Effect that this projectile will deliver. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> ProjectileGameplayEffect;

	/** Is this projectile a purely predicted projectile (on clients only)? */
	UPROPERTY()
	bool bPredictedProjectile = false;

	/** Spec Handle for the Gameplay Ability that triggered this projectile to spawn in the first place. */
	UPROPERTY()
	FGameplayAbilitySpecHandle SourceAbilityHandle;

	/** Tracks the state, network authority, and prediction status of a specific ability activation. Holds networking and prediction info. */
	UPROPERTY()
	FGameplayAbilityActivationInfo SourceActivationInfo;
};
