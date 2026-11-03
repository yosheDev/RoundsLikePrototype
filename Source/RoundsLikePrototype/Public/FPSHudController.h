// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/Drafting/DraftingUI.h"
#include "UI/HUD/PlayerHUD.h"
#include "UI/Presentation/MatchStateDisplayWidget.h"
#include "FPSGameState.h"
#include "FPSHudController.generated.h"

class UUserWidget;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API AFPSHudController : public AHUD
{
	GENERATED_BODY()
	
public:

    UFUNCTION()
    void ShowAbilitySelection();

    UFUNCTION()
    void HideAbilitySelection();
    
    // This is called when CurrentAbilityOffers is replicated to the client.
    UFUNCTION()
    void RefreshAbilitySelection();

    // If HUDWidget does not exist, create it and add it to viewport. Makes invisible by default.
    UFUNCTION()
    void CreateHUDWidget();

    UFUNCTION()
    void SetDisplayHUD(bool bShouldDisplay);

    UFUNCTION()
    void UpdateHealthHUD(float CurrentHealth, float MaxHealth);

    UFUNCTION()
    UPlayerHUD* GetHUDWidget();

    UFUNCTION()
    UDraftingUI* GetDraftingWidget();

    UFUNCTION()
    UMatchStateDisplayWidget* GetMatchStateDisplayWidget();

    UFUNCTION()
    void BeginTranslateBottlecap(uint8 BottlecapID, FBottlecapReturnLocation ReturnLocationSS, bool bIsDeallocating);

    UFUNCTION()
    const TArray<FBottlecapReturnLocation> GetBottlecapReturnLocations(uint8 Amount);
    
    UFUNCTION()
    void PresentMatchState(FGamePlayerData PlayerOneData, FGamePlayerData PlayerTwoData, uint8 CurrentLoserID);

protected:

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDraftingUI> DraftingWidgetClass;

    UPROPERTY()
    UDraftingUI* DraftingWidget;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPlayerHUD> HUDWidgetClass;

    UPROPERTY()
    UPlayerHUD* HUDWidget;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UMatchStateDisplayWidget> MatchStateDisplayWidgetClass;

    UPROPERTY()
    UMatchStateDisplayWidget* MatchStateDisplayWidget;
};
