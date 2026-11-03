// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSGameState.h"
#include "MatchStateDisplayWidget.generated.h"

class UTextBlock;
class UCanvasPanel;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UMatchStateDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Components
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* MainCanvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerOneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerOnePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerTwoName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerTwoPoints;
#pragma endregion

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Match")
	void UpdateMatchStateDisplay(FGamePlayerData PlayerOneData, FGamePlayerData PlayerTwoData, uint8 CurrentLoserID);

	UFUNCTION(BlueprintCallable, Category = "Match")
	void EndDisplay();
};
