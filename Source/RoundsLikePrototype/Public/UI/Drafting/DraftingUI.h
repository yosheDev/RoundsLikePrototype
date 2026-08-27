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
class UCanvasPanel;
struct FBottlecapReturnLocation;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UDraftingUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainCanvas;

	UPROPERTY(meta = (BindWidget))
	UButton* SelectDraftButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DraftingPlayerText;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AbilityCards;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* StatButtons;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* BottlecapSlot1Debug;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BottlecapSlot2Debug;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BottlecapSlot3Debug;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

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

	// On construction, populates with locations of bottlecaps. Used to return them to place when deallocating.
	UPROPERTY()
	TArray<FVector2D> BottlecapDefaultLocations;

	// Bottlecap Slots 1-3 and if they are currently taken up right now.
	UPROPERTY()
	TArray<bool> BottlecapIsDefaultLocationTaken;

	// Which bottlecap slot a bottlecap is currently occupying.
	UPROPERTY()
	TArray<int32> BottlecapCurrentDefaultSlots;

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
	void TranslateBottlecap(uint8 BottlecapID, FBottlecapReturnLocation TargetLocSS, bool bIsDeallocating);

	UFUNCTION()
	const TArray<FBottlecapReturnLocation> GetBottlecapReturnLocations(uint8 Amount);

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void FinishDraftClick();

private:

	UFUNCTION()
	void UpdateBottlecapLocation(UImage* Bottlecap, uint8 BottlecapID);

	// Assigned Widget IDs to the stat buttons. Cards get ID's in their refresh function.
	UFUNCTION()
	void AssignStatWidgetIDs();

	UFUNCTION()
	FVector2D GetWidgetCenterInCanvas(UWidget* Widget) const;

	UFUNCTION()
	FVector2D GetWidgetCenterInViewport(UWidget* Widget) const;

	UFUNCTION()
	FVector2D GetViewportPositionInCanvas(FVector2D ViewportPosition) const;

	bool bBottlecapDefaultLocationsInitialized = false;

	void InitializeBottlecapDefaultLocations();
};
