// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Enums/SpawnSide.h"
#include "FPSPlayerStart.generated.h"

UCLASS()
class ROUNDSLIKEPROTOTYPE_API AFPSPlayerStart : public APlayerStart
{
    GENERATED_BODY()

public:

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    ESpawnSide SpawnSide;
};
