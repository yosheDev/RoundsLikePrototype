// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/Drafting/DraftingUI.h"
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

protected:

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDraftingUI> DraftingWidgetClass;

    UPROPERTY()
    UDraftingUI* DraftingWidget;
};
