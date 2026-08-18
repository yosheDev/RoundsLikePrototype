// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/PrimaryAssetId.h"
#include "AbilityPoolSubsystem.generated.h"



UCLASS()
class ROUNDSLIKEPROTOTYPE_API UAbilityPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Ability Pool")
	void RefreshAbilityDefinitions();

	FPrimaryAssetId GetRandomAbility();

	TArray<FPrimaryAssetId> GetAbilityOffers();

public:
	TArray<FPrimaryAssetId> AbilityIDs;
};
