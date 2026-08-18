// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DraftingUI.generated.h"

class UTextBlock;
class UButton;
class UHorizontalBox;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UDraftingUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UButton* SelectDraftButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DraftingPlayerText;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AbilityCards;

public:

	UFUNCTION()
	void RefreshAbilityCards();

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleMyButtonClick();
};
