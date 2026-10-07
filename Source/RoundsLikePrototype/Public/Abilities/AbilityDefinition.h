#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "AbilityDefinition.generated.h"

UENUM(BlueprintType)
enum class EAbilityRarity : uint8
{
    Common,
    Uncommon,
    Rare,
    Legendary
};

UCLASS(BlueprintType)
class UAbilityDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag AbilityTag;

    // Name of ability.
    UPROPERTY(EditDefaultsOnly)
    FText Name;

    // Description that appears in text field under selection.
    UPROPERTY(EditDefaultsOnly)
    FText Description;

    // Description of stat changes displayed on the card itself.
    UPROPERTY(EditDefaultsOnly, meta = (MultiLine = true))
    FText ChangeDescription;

    // Flavor text that may appear at bottom of card.
    UPROPERTY(EditDefaultsOnly)
    FText FlavorText;

    UPROPERTY(EditDefaultsOnly)
    EAbilityRarity Rarity = EAbilityRarity::Common;

    UPROPERTY(EditDefaultsOnly)
    int32 Cost = 2;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayAbility>> GASAbilities;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayEffect>> GASEffects;

    // User must have these tags for this ability to appear on the draft screen for them.
    UPROPERTY(EditDefaultsOnly)
    FGameplayTagContainer DependencyAbilities;

    // If user has these tags, this ability will not appear on the draft screen for them.
    UPROPERTY(EditDefaultsOnly)
    FGameplayTagContainer BlockingAbilities;

    // If true, this ability is removed from the pool once a player has chosen it.
    UPROPERTY(EditDefaultsOnly)
    bool bIsUniqueAbility;

    // If true, this ability will not reappear if the player already has it.
    UPROPERTY(EditDefaultsOnly)
    bool bOnlyOnePerPlayer;

    // If true, never appear in the ability selection screen.
    UPROPERTY(EditDefaultsOnly)
    bool bNeverAppearInSelection;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};