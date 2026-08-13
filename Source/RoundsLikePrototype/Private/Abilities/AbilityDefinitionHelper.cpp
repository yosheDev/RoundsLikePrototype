// Copyright Jacob Jones 2026


#include "Abilities/AbilityDefinitionHelper.h"
#include "Abilities/AbilityDefinition.h"
#include "Engine/AssetManager.h"

UAbilityDefinition* AbilityDefinitions::Find(FGameplayTag AbilityTag)
{
    if (!AbilityTag.IsValid())
    {
        return nullptr;
    }

    const FPrimaryAssetId AssetId(
        FPrimaryAssetType(TEXT("AbilityDefinition")),
        AbilityTag.GetTagName()
    );

    UAbilityDefinition* Definition = UAssetManager::Get().GetPrimaryAssetObject<UAbilityDefinition>(AssetId);

    if (!Definition)
    {
        // Async load.
    }

    return Definition;
}
