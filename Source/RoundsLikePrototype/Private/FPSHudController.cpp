// Copyright Jacob Jones 2026


#include "FPSHudController.h"
#include "UI/Drafting/DraftingUI.h"

// This is causing an exception violation.
void AFPSHudController::ShowAbilitySelection()
{
    UE_LOG(LogTemp, Warning, TEXT("HUD: %s"), *GetNameSafe(this));
    UE_LOG(LogTemp, Warning, TEXT("Owning PC: %s"), *GetNameSafe(GetOwningPlayerController()));
    UE_LOG(LogTemp, Warning, TEXT("Widget Class: %s"), *GetNameSafe(DraftingWidgetClass));

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
