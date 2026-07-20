// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "Weapons/Projectiles/BulletSpec.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "BulletProjectile.generated.h"

class UNiagaraSystem;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API ABulletProjectile : public AActor
{
	GENERATED_BODY()
	
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> SphereHitCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> SphereOverlapCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UNiagaraComponent> NiagaraComponent;

#pragma endregion
public:	
	// Sets default values for this actor's properties
	ABulletProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnComponentBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void PredictDamage(AActor* Target);

	UFUNCTION()
	void ApplyDamage(AActor* Target);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Initializes attributes relevant to local projectile (movement, size, traits)
	void InitializeBulletData(FProjectileSpawnData SpawnData);

	// Initializes handle for the gameplay effect that will be delivered.
	void InitializeGameplayEffectSpec(FGameplayEffectSpecHandle InEffectSpec);

protected:
	//UPROPERTY(ReplicatedUsing = OnRep_BulletSpec)
	//FBulletSpec BulletSpec;

	UPROPERTY(ReplicatedUsing = OnRep_BulletData)
	FProjectileSpawnData BulletData;

	UFUNCTION()
	void OnRep_BulletData();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayEffectSpecHandle GameplayEffectSpec;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> ProjectileGameplayEffect;

	UPROPERTY()
	bool bPredictedProjectile = false;

	UPROPERTY()
	FGameplayAbilitySpecHandle SourceAbilityHandle;

	UPROPERTY()
	FGameplayAbilityActivationInfo SourceActivationInfo;
};
