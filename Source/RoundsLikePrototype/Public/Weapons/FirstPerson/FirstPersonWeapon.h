// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FirstPersonWeapon.generated.h"

UCLASS()
class ROUNDSLIKEPROTOTYPE_API AFirstPersonWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AFirstPersonWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	USkeletalMeshComponent* GetMesh();

	/** Weapon Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

protected:

	virtual void BeginPlay() override;

public:

	/** Binds cosmetic events to weapon delegates. */
	void BindToWeapon(TObjectPtr<class AProjectileWeapon> Weapon);

	void PlayFireWeaponMontage();
};
