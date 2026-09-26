// Copyright Jacob Jones 2026


#include "Subsystems/AbilityPoolSubsystem.h"
#include "Abilities/AbilityDefinition.h"
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

TArray<FPrimaryAssetId> UAbilityPoolSubsystem::GetAbilityOffers(const FAbilityPoolContext& Context)
{
    TArray<FPrimaryAssetId> Offers;

    TArray<FPrimaryAssetId> EligibleAbilities = GetEligibleSkills(Context);

    // Determine which rarities even have available skills.
    
    for (int32 i = 0; i < 5; ++i)
    {
        FPrimaryAssetId Selected = FPrimaryAssetId();

        if (EligibleAbilities.Num() > 0)
        {
            // Determine which rarities can currently produce an ability.
            const TArray<EAbilityRarity> AvailableRarities = GetAvailableRarities(EligibleAbilities);
            UE_LOG(LogTemp, Error, TEXT("Ability Pool: Available rarities are:"));
            for (EAbilityRarity Rarity : AvailableRarities)
            {
                UE_LOG(LogTemp, Error, TEXT("Ability Pool: Available rarity: [%s]"), *UEnum::GetValueAsString(Rarity));
            }

            // Determine which rarity the skill selected will belong to.
            EAbilityRarity Rarity = RollAbilityRarity(AvailableRarities);
            UE_LOG(LogTemp, Error, TEXT("Ability Pool: Rarity to select from: [%s]"), *UEnum::GetValueAsString(Rarity));

            // Determing the skill that will be offered.
            Selected = RandomAbilityOfRarity(Rarity, EligibleAbilities);
            UE_LOG(LogTemp, Error, TEXT("Ability Pool: Skill selected is: [%s]"), *Selected.ToString());
        }

        // Select fallback abilities if previous methods fail.
        if (!Selected.IsValid())
        {
            Selected = SelectFallbackAbility(EligibleAbilities);
        }
        if (!Selected.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Ability Pool: Worst failure, unable to select any skills."));
            break;
        }

        Offers.Add(Selected);
        UE_LOG(LogTemp, Error, TEXT("Ability Pool: Skill added to offers: [%s]"), *Selected.ToString());

        // Prevent this ability from being offered twice during the same draft.
        EligibleAbilities.Remove(Selected);
    }

    return Offers;
}

TArray<FPrimaryAssetId> UAbilityPoolSubsystem::GetEligibleSkills(const FAbilityPoolContext& Context)
{
    TArray<FPrimaryAssetId> EligibleSkills;

    for (const FPrimaryAssetId& AbilityID : AbilityIDs)
    {
        UAbilityDefinition* Definition = UAssetManager::Get().GetPrimaryAssetObject<UAbilityDefinition>(AbilityID);

        if (!Definition){ continue; }

        // If this definition is NOT meant to ever appear in skill selection.
        if (Definition->bNeverAppearInSelection) { continue; }

        // Player already owns this ability and it is limited to only one per player.
        if (Definition->bOnlyOnePerPlayer && Context.OwnedAbilities.HasTag(Definition->AbilityTag)){ UE_LOG(LogTemp, Error, TEXT("Ability Pool: [%s] is excluded. Set to one per player and player already has it."), *AbilityID.ToString());  continue; }

        // Player does not have all required dependencies.
        if (!Context.OwnedAbilities.HasAll(Definition->DependencyAbilities)){ continue; }

        // Player has an ability that prevents this ability from appearing.
        if (Context.OwnedAbilities.HasAny(Definition->BlockingAbilities)){ continue; }

        // This ability is unique and has already been claimed by another player.
        if (Definition->bIsUniqueAbility && ClaimedUniqueAbilities.Contains(AbilityID)){ continue; }

        EligibleSkills.Add(AbilityID);
    }

    return EligibleSkills;
}

TArray<EAbilityRarity> UAbilityPoolSubsystem::GetAvailableRarities(const TArray<FPrimaryAssetId>& EligibleAbilities) const
{
    TArray<EAbilityRarity> AvailableRarities;

    UAssetManager& AssetManager = UAssetManager::Get();

    for (const FPrimaryAssetId& AbilityID : EligibleAbilities)
    {
        const UAbilityDefinition* Definition = AssetManager.GetPrimaryAssetObject<UAbilityDefinition>(AbilityID);

        if (!Definition)
        {
            continue;
        }

        if (!AvailableRarities.Contains(Definition->Rarity))
        {
            AvailableRarities.Add(Definition->Rarity);
        }
    }

    return AvailableRarities;
}

EAbilityRarity UAbilityPoolSubsystem::RollAbilityRarity(const TArray<EAbilityRarity>& AvailableRarities) const
{
    const float Roll = FMath::FRandRange(0.0f, 100.0f);

    if (Roll <= RarityLegendaryPercentage)
    {
        if (AvailableRarities.Contains(EAbilityRarity::Legendary))
        {
            return EAbilityRarity::Legendary;
        }
        else
        {
            return RollAbilityRarity(AvailableRarities);
        }
    }

    else if (Roll <= RarityRarePercentage)
    {
        if (AvailableRarities.Contains(EAbilityRarity::Rare))
        {
            return EAbilityRarity::Rare;
        }
        else
        {
            return RollAbilityRarity(AvailableRarities);
        }
    }

    else if (Roll <= RarityUncommonPercentage)
    {
        if (AvailableRarities.Contains(EAbilityRarity::Uncommon))
        {
            return EAbilityRarity::Uncommon;
        }
        else
        {
            return RollAbilityRarity(AvailableRarities);
        }
    }

    return EAbilityRarity::Common;
}

FPrimaryAssetId UAbilityPoolSubsystem::RandomAbilityOfRarity(EAbilityRarity Rarity, const TArray<FPrimaryAssetId>& EligibleAbilities) const
{
    TArray<FPrimaryAssetId> MatchingAbilities;

    UAssetManager& AssetManager = UAssetManager::Get();

    for (const FPrimaryAssetId& AbilityID : EligibleAbilities)
    {
        const UAbilityDefinition* Definition = AssetManager.GetPrimaryAssetObject<UAbilityDefinition>(AbilityID);

        if (!Definition){ continue; }

        if (Definition->Rarity == Rarity)
        {
            MatchingAbilities.Add(AbilityID);
        }
    }

    if (MatchingAbilities.Num() == 0)
    {
        // As a fallback, get a random elibile ability.
        UE_LOG(LogTemp, Error, TEXT("Ability Pool: No ability of the desired rarity was found. Returning a random eligible ability."));
        
        FPrimaryAssetId RandomID = EligibleAbilities[0, (EligibleAbilities.Num() - 1)];
        if (RandomID.IsValid())
        {
            return RandomID;
        }
        else
        {
            return FPrimaryAssetId();
        }
    }

    const int32 RandomIndex = FMath::RandRange(0, MatchingAbilities.Num() - 1);

    return MatchingAbilities[RandomIndex];
}

FPrimaryAssetId UAbilityPoolSubsystem::SelectFallbackAbility(const TArray<FPrimaryAssetId>& EligibleAbilities) const
{
    UE_LOG(LogTemp, Error, TEXT("Ability Pool: Selecting fallback ability."));

    if (EligibleAbilities.Num() == 0)
    { 
        UE_LOG(LogTemp, Error, TEXT("Ability Pool: No eligible abilities. Selecting completely random ability."));
        // At this point, genuinely just return a random common ability. Those will probably be the most stackable without breaking stuff.
        return RandomAbilityOfRarity(EAbilityRarity::Common, AbilityIDs);
    }

    const int32 RandomIndex = FMath::RandRange(0, EligibleAbilities.Num() - 1);

    return EligibleAbilities[RandomIndex];
}

