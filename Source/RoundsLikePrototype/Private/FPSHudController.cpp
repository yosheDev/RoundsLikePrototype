// Copyright Jacob Jones 2026


#include "FPSHudController.h"
#include "UI/Drafting/DraftingUI.h"
#include "UI/HUD/PlayerHUD.h"

// This is causing an exception violation.
void AFPSHudController::ShowAbilitySelection()
{
    /*UE_LOG(LogTemp, Warning, TEXT("HUD: %s"), *GetNameSafe(this));
    UE_LOG(LogTemp, Warning, TEXT("Owning PC: %s"), *GetNameSafe(GetOwningPlayerController()));*/

    if (!DraftingWidget)
    {
        DraftingWidget = CreateWidget<UDraftingUI>(GetOwningPlayerController(), DraftingWidgetClass);

        if (IsValid(DraftingWidget))
        {
            DraftingWidget->AddToViewport();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create DraftingWidget"));
            return;
        }
    }


    DraftingWidget->SetVisibility(ESlateVisibility::Visible);
}

void AFPSHudController::HideAbilitySelection()
{
    if (DraftingWidget)
    {
        DraftingWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void AFPSHudController::RefreshAbilitySelection()
{
    if (DraftingWidget)
    {
        DraftingWidget->RefreshAbilityCards();
    }
}

void AFPSHudController::SetDisplayHUD(bool bShouldDisplay)
{
    if (!HUDWidget)
    {
        HUDWidget = CreateWidget<UPlayerHUD>(GetOwningPlayerController(), HUDWidgetClass);

        if (IsValid(HUDWidget))
        {
            HUDWidget->AddToViewport();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create HUDWidget"));
            return;
        }
    }

    HUDWidget->SetVisibility(bShouldDisplay ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void AFPSHudController::UpdateHealthHUD(float CurrentHealth, float MaxHealth)
{
    HUDWidget->UpdateHealthBar(CurrentHealth, MaxHealth);
}
