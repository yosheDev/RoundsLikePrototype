// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IWeapon.generated.h"

class AProjectileWeapon;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeapon : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Common interface for Shooter Game weapon holder classes
 */
class ROUNDSLIKEPROTOTYPE_API IWeapon
{
	GENERATED_BODY()

public:

	/** Attaches a weapon's meshes to the owner */
	//virtual void AttachWeaponMeshes(AProjectileWeapon* Weapon) = 0;
};