// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "DraftingUI.generated.h"

class UTextBlock;
class UButton;
class UHorizontalBox;
class UImage;

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

	// <BottlecapSprite, bIsBottlecapAllocated>
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TMap<UImage*, bool> Bottlecaps;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TArray<TObjectPtr<UImage>> AvailableBottlecaps;

	UPROPERTY(meta = (BindWidget))
	UImage* Bottlecap1;

	UPROPERTY(meta = (BindWidget))
	UImage* Bottlecap2;

	UPROPERTY(meta = (BindWidget))
	UImage* Bottlecap3;

	#pragma region Bottlecap Translation

	UPROPERTY()
	float BottlecapTranslationSpeed = 1000.0f;

	// Calculated before any movements to maintain consistent travel speeds.
	UPROPERTY()
	TArray<float> BottlecapTranslationDurations;

	// Recalculates when initiating a new movement timer.
	UPROPERTY()
	TArray<FVector2D> BottlecapStartLocations;

	// Recalculates when initiating a new movement timer.
	UPROPERTY()
	TArray<FVector2D> BottlecapEndLocations;

	// Store timer handles for BottlecapIDs.
	UPROPERTY()
	TArray<FTimerHandle> BottlecapTranslationTimers;

	// Stores elapsed times for BottlecapIDs.
	UPROPERTY()
	TArray<float> BottlecapTimerElapsedTimes;

	#pragma endregion
public:

	UFUNCTION()
	void RefreshAbilityCards();

	UFUNCTION()
	void TranslateBottlecap(uint8 BottlecapID, FVector2D TargetLocSS);

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void FinishDraftClick();

private:

	UFUNCTION()
	void UpdateBottlecapLocation(UImage* Bottlecap, uint8 BottlecapID);
};
