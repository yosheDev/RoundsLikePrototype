// Copyrighted Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/IWeapon.h"
#include "ProjectileWeapon.generated.h"

// To do: Make interface for interacting with owner

UCLASS()
class ROUNDSLIKEPROTOTYPE_API AProjectileWeapon : public AActor, public IWeapon
{
	GENERATED_BODY()
	
	/** First person perspective mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

protected:
	
	// Pointer to weapon holder via interface

public:	
	// Sets default values for this actor's properties
	AProjectileWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual TArray<FTransform> GetMuzzleLocations() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void PrimaryFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
