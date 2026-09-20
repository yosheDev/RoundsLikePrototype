// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Enums/WeaponFireType.h"
#include "FireData.generated.h"


USTRUCT(BlueprintType)
struct FFireData
{
	GENERATED_BODY()

	// 0 = Single, 1 = Burst, 2 = Auto, 3 = Spread, 4 = Cannon?
	EWeaponFireType WeaponFireType = EWeaponFireType::Single;

	// Amount of bullets to be fired within a burst.
	int32 FireBurstAmount = 1;

	// Interval between burst fire shots.
	float FireBurstInterval = .2;

	// Amount of bullets to be spread in this shot.
	int32 FireBulletAmount = 1;

	// X-Axis angle offset range for the fire to spread across.
	float FireSpreadXAngle = -1.0f;

	// Z-Axis angle offset range for the fire to spread across.
	float FireSpreadZAngle = -1.0f;
};
