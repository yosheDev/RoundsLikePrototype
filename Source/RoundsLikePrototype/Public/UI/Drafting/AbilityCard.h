// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Abilities/AbilityDefinition.h"
#include "GameplayTagContainer.h"
#include "UI/Drafting/AllocationWidgetIDInterface.h"
#include "AbilityCard.generated.h"

class UTextBlock;
class UButton;
class UImage;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UAbilityCard : public UUserWidget, public IAllocationWidgetIDInterface
{
	GENERATED_BODY()

public:

	virtual int32 GetWidgetID_Implementation() override;

	virtual void SetWidgetID_Implementation(int32 NewID) override;

	int32 WidgetID = -1;

protected:

	virtual void NativeConstruct() override;

	//UFUNCTION()
	//void HandleMyButtonClick();

private:

	void GiveAbilityToPlayer();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilityDefinition> AbilityDataAsset;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* SelectAbilityButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> AbilityDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> AbilityImage;

private:

	UPROPERTY()
	bool bIsAllocated = false;

	UFUNCTION()
	void SelectAbility();

	UPROPERTY(EditAnywhere)
	uint8 Cost = 2;
};
