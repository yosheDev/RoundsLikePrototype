// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "ProjectileInitData.generated.h"

USTRUCT(BlueprintType)
struct FProjectileInitData
{
    GENERATED_BODY()

    UPROPERTY()
    FTransform SpawnTransform;
};
