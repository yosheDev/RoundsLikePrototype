// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "Weapons/Projectiles/BulletSpec.h"
#include "BulletProjectile.generated.h"

class UNiagaraSystem;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API ABulletProjectile : public AActor
{
	GENERATED_BODY()
	
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> SphereCollision;

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeSpec(FBulletSpec BulletSpec);
};
