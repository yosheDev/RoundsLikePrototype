// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/Drafting/DraftingUI.h"
#include "UI/HUD/PlayerHUD.h"
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

    UFUNCTION()
    void SetDisplayHUD(bool bShouldDisplay);

    UFUNCTION()
    void UpdateHealthHUD(float CurrentHealth, float MaxHealth);

protected:

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDraftingUI> DraftingWidgetClass;

    UPROPERTY()
    UDraftingUI* DraftingWidget;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPlayerHUD> HUDWidgetClass;

    UPROPERTY()
    UPlayerHUD* HUDWidget;
};
