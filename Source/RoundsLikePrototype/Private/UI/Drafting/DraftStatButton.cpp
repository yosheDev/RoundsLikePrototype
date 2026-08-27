// Copyright Jacob Jones 2026


#include "UI/Drafting/DraftStatButton.h"
#include "UI/Drafting/BottlecapReturnLocation.h"
#include "Components/Button.h"
#include "FPSGameState.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"

void UDraftStatButton::NativeConstruct()
{
    Super::NativeConstruct();

    if (StatButton)
    {
        StatButton->OnClicked.AddDynamic(this, &UDraftStatButton::ClickButton);
    }
}

int32 UDraftStatButton::GetWidgetID_Implementation()
{
    return WidgetID;
}

void UDraftStatButton::SetWidgetID_Implementation(int32 NewID)
{
    WidgetID = NewID;
}

void UDraftStatButton::ClickButton_Implementation()
{
    if (!bIsAllocated)
    {
        bool bResult = TryAllocation();
        bIsAllocated = bResult;

        if (bResult)
        {
            // Yes-siree SFX
        }
        else
        {
            // Nuh-uh SFX
        }
    }
    else
    {
        bool bResult = TryDeallocation();
        bIsAllocated = !bResult;

        if (bResult)
        {
            // Yes-siree SFX
        }
        else
        {
            // Nuh-uh SFX
        }
    }
}

bool UDraftStatButton::TryAllocation()
{
    if (UWorld* World = GetWorld())
    {
        AFPSGameState* FPSGameState = World->GetGameState<AFPSGameState>();
        if (FPSGameState)
        {
            if (FPSGameState->EconomyComponent->CanAllocateBottlecaps(Cost))
            {
                TArray<FBottlecapReturnLocation> Locations;
                for (int i = 0; i < Cost; i++)
                {
                    // Get target destinations for bottlecaps to slide to.
                    FGeometry CachedGeometry = StatButton->GetCachedGeometry();
                    FVector2D LocalLocalCenter = CachedGeometry.GetLocalSize() * 0.5f;
                    FVector2D AbsoluteScreenPosition = CachedGeometry.GetAccumulatedRenderTransform().TransformPoint(LocalLocalCenter);

                    FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(StatButton);
                    FVector2D ViewportPosition = USlateBlueprintLibrary::AbsoluteToLocal(ViewportGeometry, AbsoluteScreenPosition);

                    FBottlecapReturnLocation NewReturnLocation;
                    NewReturnLocation.SlotIndex = -1;
                    NewReturnLocation.Location = ViewportPosition;
                    Locations.Add(NewReturnLocation);
                }

                if (Locations.Num() != Cost)
                {
                    return false;
                }

                FPSGameState->EconomyComponent->AllocateBottlecaps(Cost, WidgetID, Locations);
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

bool UDraftStatButton::TryDeallocation()
{
    if (UWorld* World = GetWorld())
    {
        AFPSGameState* FPSGameState = World->GetGameState<AFPSGameState>();
        if (FPSGameState)
        {
            FPSGameState->EconomyComponent->DeallocateBottlecaps(WidgetID);
            return true;
        }
    }
    return false;
}

