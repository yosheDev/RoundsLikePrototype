#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "AbilityDefinition.generated.h"

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
    TArray<TSubclassOf<UGameplayAbility>> GASAbilities;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayEffect>> Effects;

    UPROPERTY(EditDefaultsOnly)
    TArray<FGameplayTag> RequiredTags;

    FPrimaryAssetId GetPrimaryAssetID() const;
};