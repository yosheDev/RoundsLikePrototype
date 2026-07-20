// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectiles/BulletSpec.h"
#include "ProjectileSpawnData.generated.h"

USTRUCT(BlueprintType)
struct FProjectileSpawnData
{
    GENERATED_BODY()

    UPROPERTY()
    FTransform SpawnTransform;

    UPROPERTY()
    FBulletSpec BulletSpec;
};
