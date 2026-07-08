
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
	USkeletalMeshComponent* FirstPersonMesh;

	/** Third person perspective mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ThirdPersonMesh;

protected:
	
	// Pointer to weapon holder via interface

public:	
	// Sets default values for this actor's properties
	AProjectileWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
