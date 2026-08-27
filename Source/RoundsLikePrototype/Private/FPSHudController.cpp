// Copyright Jacob Jones 2026


#include "FPSHudController.h"
#include "UI/Drafting/DraftingUI.h"
#include "UI/Drafting/BottlecapReturnLocation.h"
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
    if (HUDWidget)
    {
        HUDWidget->UpdateHealthBar(CurrentHealth, MaxHealth);
    }
}

UPlayerHUD* AFPSHudController::GetHUDWidget()
{
    if (HUDWidget)
    {
        return HUDWidget;
    }
    else
    {
        return nullptr;
    }
}

void AFPSHudController::BeginTranslateBottlecap(uint8 BottlecapID, FBottlecapReturnLocation ReturnLocationSS, bool bIsDeallocating)
{
    UE_LOG(LogTemp, Error, TEXT("HUD BeginTranslateBottlecap %d"), BottlecapID);
    DraftingWidget->TranslateBottlecap(BottlecapID, ReturnLocationSS, bIsDeallocating);
}

const TArray<FBottlecapReturnLocation> AFPSHudController::GetBottlecapReturnLocations(uint8 Amount)
{
    return DraftingWidget->GetBottlecapReturnLocations(Amount);
}