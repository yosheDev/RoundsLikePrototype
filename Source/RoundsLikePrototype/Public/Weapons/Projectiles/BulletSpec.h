// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BulletSpec.generated.h"

USTRUCT(BlueprintType)
struct FBulletSpec
{
    GENERATED_BODY()

    UPROPERTY()
    float BulletDamage = 20.f;

    UPROPERTY()
    float BulletSpeed = 1200.f;

    UPROPERTY()
    float BulletGravity = 1.f;

    UPROPERTY()
    FGameplayTagContainer Tags;
};
