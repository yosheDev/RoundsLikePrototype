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

	/** Component that handles projectile velocities of movement. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	/** The primary Visual Effect of the projectile. Acts as the main visual body. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UNiagaraComponent> NiagaraComponent;

#pragma endregion

public:	

	ABulletProjectile();
	
	//virtual void Tick(float DeltaTime) override;

	/** Initializes attributes relevant to local projectile(movement, size, traits) */
	void InitializeBulletData(FProjectileSpawnData SpawnData);

	/** Initializes handle for the gameplay effect that will be delivered. */
	//void InitializeGameplayEffectSpec(FGameplayEffectSpecHandle InEffectSpec);

protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnComponentHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		FVector NormalImpulse, 
		const FHitResult& Hit);

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

	// NOTES
	// I don't really have prediction functioning practically.
	//The FPredictionKey handles the synchronization of ability execution (e.g., starting an animation montage or firing a weapon) 
	// between the local client and the server. It ties the locally predicted action to the server's acknowledgment so effects aren't duplicated.
};
