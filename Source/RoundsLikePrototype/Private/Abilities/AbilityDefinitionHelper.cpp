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

    Find(AssetId, MoveTemp(CompletionCallback));
}

void AbilityDefinitions::Find(
    const FPrimaryAssetId& AssetId,
    TFunction<void(UAbilityDefinition*)> CompletionCallback)
{
    if (!AssetId.IsValid())
    {
        CompletionCallback(nullptr);
        return;
    }

    UAssetManager& AssetManager = UAssetManager::Get();

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

                CompletionCallback(Definition);
            }
        )
    );
}

void AbilityDefinitions::Scan()
{
    UAssetManager& AssetManager = UAssetManager::Get();

    const FPrimaryAssetType AbilityDefinitionType(TEXT("AbilityDefinition"));
    TArray<FPrimaryAssetId> AbilityIDs;

    AssetManager.GetPrimaryAssetIdList(AbilityDefinitionType, AbilityIDs);

    // Ensure AbilityDefinition assets have been scanned.
    AssetManager.ScanPathsForPrimaryAssets(
        FPrimaryAssetType(TEXT("AbilityDefinition")),
        { TEXT("/Game/Abilities/Data") },
        UAbilityDefinition::StaticClass(),
        false,
        false,
        true
    );
}

void AbilityDefinitions::Load(
    const TArray<FPrimaryAssetId>& AssetIds,
    TFunction<void(const TArray<UAbilityDefinition*>&)> CompletionCallback)
{
    if (AssetIds.Num() == 0)
    {
        CompletionCallback({});
        return;
    }

    UAssetManager& AssetManager = UAssetManager::Get();

    AssetManager.LoadPrimaryAssets(
        AssetIds,
        {},
        FStreamableDelegate::CreateLambda(
            [AssetIds, CompletionCallback = MoveTemp(CompletionCallback)]()
            {
                UAssetManager& AssetManager = UAssetManager::Get();

                TArray<UAbilityDefinition*> Definitions;
                Definitions.Reserve(AssetIds.Num());

                for (const FPrimaryAssetId& AssetId : AssetIds)
                {
                    Definitions.Add(AssetManager.GetPrimaryAssetObject<UAbilityDefinition>(AssetId));
                }

                CompletionCallback(Definitions);
            }
        )
    );
}