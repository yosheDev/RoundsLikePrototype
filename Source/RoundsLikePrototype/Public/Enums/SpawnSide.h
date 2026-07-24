// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "SpawnSide.generated.h"

UENUM(BlueprintType)
enum class ESpawnSide : uint8
{
    Red UMETA(DisplayName = "Red"),
    Blue UMETA(DisplayName = "Blue")
};
