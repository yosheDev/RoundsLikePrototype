// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UAbilityDefinition;

namespace AbilityDefinitions
{
    void Find(FGameplayTag AbilityTag, TFunction<void(UAbilityDefinition*)> CompletionCallback);
}
