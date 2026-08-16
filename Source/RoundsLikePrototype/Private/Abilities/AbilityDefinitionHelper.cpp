// Copyright Jacob Jones 2026

#include "Abilities/AbilityDefinitionHelper.h"
#include "Abilities/AbilityDefinition.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void AbilityDefinitions::Find(
    FGameplayTag AbilityTag,
    TFunction<void(UAbilityDefinition*)> CompletionCallback)
{
    if (!AbilityTag.IsValid())
    {
        CompletionCallback(nullptr);
        return;
    }

    const FPrimaryAssetId AssetId(
        FPrimaryAssetType(TEXT("AbilityDefinition")),
        AbilityTag.GetTagName()
    );

    UAssetManager& AssetManager = UAssetManager::Get();

    //UE_LOG(
    //    LogTemp,
    //    Warning,
    //    TEXT("AbilityDefinitions: Looking for [%s]"),
    //    *AssetId.ToString()
    //);

    // Ensure AbilityDefinition assets have been scanned.
    AssetManager.ScanPathsForPrimaryAssets(
        FPrimaryAssetType(TEXT("AbilityDefinition")),
        { TEXT("/Game/Abilities/Data") },
        UAbilityDefinition::StaticClass(),
        false,
        false,
        true
    );

    if (UAbilityDefinition* Definition =
        AssetManager.GetPrimaryAssetObject<UAbilityDefinition>(AssetId))
    {
        CompletionCallback(Definition);
        return;
    }

    AssetManager.LoadPrimaryAsset(
        AssetId,
        {},
        FStreamableDelegate::CreateLambda(
            [AssetId, CompletionCallback = MoveTemp(CompletionCallback)]()
            {
                UAbilityDefinition* Definition =
                    UAssetManager::Get()
                    .GetPrimaryAssetObject<UAbilityDefinition>(AssetId);

                /*UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("AbilityDefinitions: Load completed for [%s], Definition = %s"),
                    *AssetId.ToString(),
                    *GetNameSafe(Definition)
                );*/

                CompletionCallback(Definition);
            }
        )
    );
}