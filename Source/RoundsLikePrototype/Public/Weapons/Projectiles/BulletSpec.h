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
    float BulletSpeed = 1200.f;

    // Maximum displacement from the initial fire angle forward.
    UPROPERTY()
    float BulletArc = 100.0f;

    UPROPERTY()
    float BulletGravity = 1.f;

    // Distance at which the designed arc returns to initial fire angle forward.
    UPROPERTY()
    float BulletArcDistance = 500.0f;

    // How strongly the designed arc is reduced when aiming vertically.
    UPROPERTY()
    float BulletArcPitchInfluence = 1.0f;

    UPROPERTY()
    FGameplayTagContainer Tags;
};
