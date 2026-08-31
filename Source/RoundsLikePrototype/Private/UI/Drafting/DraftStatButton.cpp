// Copyright Jacob Jones 2026


#include "UI/Drafting/DraftStatButton.h"
#include "UI/Drafting/BottlecapReturnLocation.h"
#include "UI/Drafting/DraftingUI.h"
#include "Components/Button.h"
#include "FPSGameState.h"
#include "FPSPlayerState.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"

void UDraftStatButton::NativeConstruct()
{
    Super::NativeConstruct();

    UUserWidget* ParentWidget = GetTypedOuter<UUserWidget>();
    if (ParentWidget)
    {
        DraftingUI = Cast<UDraftingUI>(ParentWidget);
    }

    if (StatButton)
    {
        StatButton->OnClicked.AddDynamic(this, &UDraftStatButton::ClickButton);
    }

    // Bind AllocationSucceeded Delegate
    if (AFPSPlayerState* PS = GetOwningPlayer()->GetPlayerState<AFPSPlayerState>())
    {
        PS->OnAllocationSucceeded.AddUObject(
            this,
            &UDraftStatButton::HandleAllocationSucceeded);
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
        TryAllocation();
        //bIsAllocated only set to true if OnAllocationSucceeded delegate returns.
    }
    else
    {
        TryDeallocation();
        bIsAllocated = false; // Deallocation always succeeds, so set to false.
    }
}

void UDraftStatButton::TryAllocation()
{
    #pragma region Request Allocation
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

    AFPSPlayerState* PS = GetOwningPlayer()->GetPlayerState<AFPSPlayerState>();
    if (PS)
    {
        PS->Server_RequestAllocateBottlecaps(Cost, WidgetID, Locations);
    }
    #pragma endregion
}

void UDraftStatButton::HandleAllocationSucceeded(int32 InWidgetID)
{
    if (InWidgetID != WidgetID)
    {
        return;
    }

    bIsAllocated = true;

    if (DraftingUI)
    {
        DraftingUI->SetWidgetSelected(Cast<UUserWidget>(this), true);
    }
}

void UDraftStatButton::TryDeallocation()
{
    AFPSPlayerState* PS = GetOwningPlayer()->GetPlayerState<AFPSPlayerState>();

    if (PS)
    {
        PS->Server_RequestDeallocateBottlecaps(WidgetID);
    }

    if (DraftingUI)
    {
        DraftingUI->SetWidgetSelected(Cast<UUserWidget>(this), false);
    }
}

void UDraftStatButton::ApplyStatBuffToPlayer()
{
    if (!AbilityDataAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("Stat Button had no AbilityDataAsset!"));
        return;
    }

    AFPSGameState* GS = GetWorld()->GetGameState<AFPSGameState>();

    if (!GS)
    {
        return;
    }

    AFPSPlayerState* LoserState = GS->GetCurrentLoserState();

    if (!LoserState)
    {
        return;
    }

    // Is this valid on the client?
    LoserState->Server_AddAccruedAbility(AbilityDataAsset->AbilityTag);
}

