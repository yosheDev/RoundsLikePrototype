// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "WeaponFireType.generated.h"

UENUM(BlueprintType)
enum class EWeaponFireType : uint8
{
	Single,
	Burst,
	Auto,
	Spread,
	Cannon
};
