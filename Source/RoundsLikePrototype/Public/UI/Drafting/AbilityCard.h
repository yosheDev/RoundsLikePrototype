// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "Blueprint/UserWidget.h"
#include "Abilities/AbilityDefinition.h"
#include "GameplayTagContainer.h"
#include "UI/Drafting/AllocationWidgetIDInterface.h"
#include "AbilityCard.generated.h"

class UDraftingUI;
class UTextBlock;
class URichTextBlock;
class UButton;
class UImage;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UAbilityCard : public UUserWidget, public IAllocationWidgetIDInterface
{
	GENERATED_BODY()

public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Initializes card display to match the AbilityDataAsset assigned.
	void InitializeCard(UAbilityDefinition* NewDataAsset);

	void GiveAbilityToPlayer();

	virtual int32 GetWidgetID_Implementation() override;

	virtual void SetWidgetID_Implementation(int32 NewID) override;

	int32 WidgetID = -1;

protected:

	virtual void NativeConstruct() override;

	TObjectPtr<UDraftingUI> DraftingUI;

	UFUNCTION()
	void HandleButtonHovered();

	UFUNCTION()
	void HandleButtonUnhovered();

	#pragma region Hover Animation
public:

	// Sets scale to hovered.
	UFUNCTION()
	void SetHoveredScale();

	// Sets scale to unhovered.
	UFUNCTION()
	void SetUnhoveredScale();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* IdleAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* IdleHoveredAnimation;

protected:

	float CurrentScale = 1.0f;
	float TargetScale = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Hover")
	float NormalScale = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Hover")
	float HoverScale = 1.15f;

	UPROPERTY(EditAnywhere, Category = "Hover")
	float HoverInterpSpeed = 12.0f;
	#pragma endregion

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UAbilityDefinition> AbilityDataAsset;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCardColorByRarity(EAbilityRarity Rarity);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* SelectAbilityButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<URichTextBlock> AbilityDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> AbilityFlavor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> AbilityImage;

private:

	UPROPERTY()
	bool bIsAllocated = false;

	UFUNCTION()
	void SelectAbility();

	void HandleAllocationSucceeded(int32 InWidgetID);

	UPROPERTY()
	int32 Cost = 2;

	UFUNCTION()
	void TryAllocation();

	UFUNCTION()
	void TryDeallocation();
};
