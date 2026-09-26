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

    UPROPERTY(EditDefaultsOnly)
    FText Name;

    UPROPERTY(EditDefaultsOnly)
    FText Description;

    UPROPERTY(EditDefaultsOnly)
    EAbilityRarity Rarity = EAbilityRarity::Common;

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