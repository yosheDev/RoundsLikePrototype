// Copyright Jacob Jones 2026


#include "Subsystems/AbilityPoolSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UObject/PrimaryAssetId.h"

void UAbilityPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    RefreshAbilityDefinitions();
}

void UAbilityPoolSubsystem::RefreshAbilityDefinitions()
{
    AbilityIDs.Reset();

    UAssetManager& AssetManager = UAssetManager::Get();

    AssetManager.GetPrimaryAssetIdList(
        FPrimaryAssetType(TEXT("AbilityDefinition")),
        AbilityIDs
    );
}

TArray<FPrimaryAssetId> UAbilityPoolSubsystem::GetAbilityOffers()
{
    TArray<FPrimaryAssetId> Offers;

    for (int i = 0; i < 5; i++)
    {
        Offers.Add(GetRandomAbility());
    }
    
    return Offers;
}

FPrimaryAssetId UAbilityPoolSubsystem::GetRandomAbility()
{
    FPrimaryAssetId ID(FPrimaryAssetType(TEXT("AbilityDefinition")), FName(TEXT("DA_SuperSpeedTest")));
    return ID;
}

// System not built yet, here is kinda what I want though:
// Get all Abilities
// Filter Abilities by elegibility
// Weighting by rarity
// Random selection

