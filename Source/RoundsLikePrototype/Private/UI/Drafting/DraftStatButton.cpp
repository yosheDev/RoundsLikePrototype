// Copyright Jacob Jones 2026


#include "UI/Drafting/DraftStatButton.h"
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

void UDraftStatButton::ClickButton_Implementation()
{
    if (!bIsAllocated)
    {
        bool bResult = TryAllocation();

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
                TArray<FVector2D> Locations;
                for (int i = 0; i < Cost; i++)
                {
                    // Get target destinations for bottlecaps to slide to.
                    FGeometry CachedGeometry = StatButton->GetCachedGeometry();
                    FVector2D LocalLocalCenter = CachedGeometry.GetLocalSize() * 0.5f;
                    FVector2D AbsoluteScreenPosition = CachedGeometry.GetAccumulatedRenderTransform().TransformPoint(LocalLocalCenter);

                    FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(StatButton);
                    FVector2D ViewportPosition = USlateBlueprintLibrary::AbsoluteToLocal(ViewportGeometry, AbsoluteScreenPosition);

                    UE_LOG(LogTemp, Log, TEXT("Adding to Locations: [%s]"), *ViewportPosition.ToString());
                    Locations.Add(ViewportPosition);
                }

                if (Locations.Num() != Cost)
                {
                    return false;
                }

                FPSGameState->EconomyComponent->AllocateBottlecaps(Cost, Locations);
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
            //TArray<FVector2D> Locations;
            //for (int i = 0; i < Cost; i++)
            //{
            //    // Get target destinations for bottlecaps to slide to.
            //    FGeometry CachedGeometry = StatButton->GetCachedGeometry();
            //    FVector2D LocalLocalCenter = CachedGeometry.GetLocalSize() * 0.5f;
            //    FVector2D AbsoluteScreenPosition = CachedGeometry.GetAccumulatedRenderTransform().TransformPoint(LocalLocalCenter);

            //    FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(StatButton);
            //    FVector2D ViewportPosition = USlateBlueprintLibrary::AbsoluteToLocal(ViewportGeometry, AbsoluteScreenPosition);

            //    Locations.Add(ViewportPosition);
            //}

            //if (Locations.Num() != Cost)
            //{
            //    return false;
            //}

            //FPSGameState->EconomyComponent->DeallocateBottlecaps();
            //return true;
        }
    }
    return false;
}

