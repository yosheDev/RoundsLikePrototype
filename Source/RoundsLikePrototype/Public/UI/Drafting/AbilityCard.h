// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Abilities/AbilityDefinition.h"
#include "AbilityCard.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UAbilityCard : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	//UFUNCTION()
	//void HandleMyButtonClick();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilityDefinition> AbilityDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* SelectAbilityButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> AbilityDef;
};
