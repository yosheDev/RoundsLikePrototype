// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/PrimaryAssetId.h"

class UAbilityDefinition;

namespace AbilityDefinitions
{
    void Find(FGameplayTag AbilityTag, TFunction<void(UAbilityDefinition*)> CompletionCallback);

    void Find(const FPrimaryAssetId& AssetId, TFunction<void(UAbilityDefinition*)> CompletionCallback);

    void Scan();

    void Load(const TArray<FPrimaryAssetId>& AssetIds, TFunction<void(const TArray<UAbilityDefinition*>&)> CompletionCallback);
}
