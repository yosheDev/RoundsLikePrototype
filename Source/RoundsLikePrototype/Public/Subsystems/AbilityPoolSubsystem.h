// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/PrimaryAssetId.h"
#include "Abilities/AbilityDefinition.h"
#include "GameplayTagContainer.h"
#include "AbilityPoolSubsystem.generated.h"


USTRUCT(BlueprintType)
struct FAbilityPoolContext
{
	GENERATED_BODY()

	// The abilities owned by the player the AbilityPool will be generating abilities for.
	UPROPERTY()
	FGameplayTagContainer OwnedAbilities;

	// Amount of skills to generate offers for (5 default).
	UPROPERTY()
	int32 OfferCount = 5;
};

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UAbilityPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
#pragma region Rarity Odds
protected:
	// Must be between 0-100.
	float RarityCommonPercentage{ 60.0f };
	float RarityUncommonPercentage{ 25.0f};
	float RarityRarePercentage{ 12.0f };
	float RarityLegendaryPercentage{ 3.0f };
#pragma endregion

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Ability Pool")
	void RefreshAbilityDefinitions();

	FPrimaryAssetId GetRandomAbility();

	TArray<FPrimaryAssetId> GetAbilityOffers(const FAbilityPoolContext& Context);

public:
	TArray<FPrimaryAssetId> AbilityIDs;

	TSet<FPrimaryAssetId> ClaimedUniqueAbilities;

protected:
	TArray<FPrimaryAssetId> GetEligibleSkills(const FAbilityPoolContext& Context);

	TArray<EAbilityRarity> GetAvailableRarities(const TArray<FPrimaryAssetId>& EligibleAbilities) const;

	EAbilityRarity RollAbilityRarity(const TArray<EAbilityRarity>& AvailableRarities) const;

	FPrimaryAssetId RandomAbilityOfRarity(EAbilityRarity Rarity, const TArray<FPrimaryAssetId>& EligibleAbilities) const;

	FPrimaryAssetId SelectFallbackAbility(const TArray<FPrimaryAssetId>& EligibleAbilities) const;
};
