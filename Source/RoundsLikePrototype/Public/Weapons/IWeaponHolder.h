// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IWeaponHolder.generated.h"

class AProjectileWeapon;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponHolder : public UInterface
{
	GENERATED_BODY()
};

class ROUNDSLIKEPROTOTYPE_API IWeaponHolder
{
	GENERATED_BODY()

public:
	
	/** Equip the supplied weapon */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapons|WeaponHolder")
	void CreateAndEquipWeapon(TSubclassOf<AProjectileWeapon> WeaponClass);

	/** Get the current weapon */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapons|WeaponHolder")
	AProjectileWeapon* GetEquippedWeapon() const;
};
