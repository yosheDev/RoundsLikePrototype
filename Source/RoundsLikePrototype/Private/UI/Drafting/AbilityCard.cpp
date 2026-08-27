// Copyright Jacob Jones 2026


#include "UI/Drafting/AbilityCard.h"
#include "GameplayTagContainer.h"
#include "FPSPlayerState.h"
#include "FPSGameState.h"
#include "UI/Drafting/BottlecapReturnLocation.h"
#include "Abilities/AbilityDefinition.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"

void UAbilityCard::NativeConstruct()
{
    Super::NativeConstruct();

    if (SelectAbilityButton)
    {
        SelectAbilityButton->OnClicked.AddDynamic(this, &UAbilityCard::SelectAbility);
    }

    AbilityName->SetText(AbilityDataAsset->Name);
    AbilityDesc->SetText(AbilityDataAsset->Description);

    // Bind AllocationSucceeded Delegate
    if (AFPSPlayerState* PS = GetOwningPlayer()->GetPlayerState<AFPSPlayerState>())
    {
        PS->OnAllocationSucceeded.AddUObject(
            this,
            &UAbilityCard::HandleAllocationSucceeded);
    }
}

int32 UAbilityCard::GetWidgetID_Implementation()
{
    return WidgetID;
}

void UAbilityCard::SetWidgetID_Implementation(int32 NewID)
{
    WidgetID = NewID;
}

void UAbilityCard::SelectAbility()
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

void UAbilityCard::TryAllocation()
{
    TArray<FBottlecapReturnLocation> Locations;
    for (int i = 0; i < Cost; i++)
    {
        // Get target destinations for bottlecaps to slide to.
        FGeometry CachedGeometry = SelectAbilityButton->GetCachedGeometry();
        FVector2D LocalLocalCenter = CachedGeometry.GetLocalSize() * 0.5f;
        FVector2D AbsoluteScreenPosition = CachedGeometry.GetAccumulatedRenderTransform().TransformPoint(LocalLocalCenter);

        FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(SelectAbilityButton);
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
    // TO DO: Do not apply this on click. Only apply selected cards when advancing past drafting screen.
    GiveAbilityToPlayer();
}

void UAbilityCard::HandleAllocationSucceeded(int32 InWidgetID)
{
    if (InWidgetID != WidgetID)
    {
        return;
    }

    bIsAllocated = true;
}

void UAbilityCard::TryDeallocation()
{
    AFPSPlayerState* PS = GetOwningPlayer()->GetPlayerState<AFPSPlayerState>();

    if (PS)
    {
        PS->Server_RequestDeallocateBottlecaps(WidgetID);
    }
}

void UAbilityCard::GiveAbilityToPlayer()
{
    if (!AbilityDataAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("Ability Card had no AbilityDataAsset!"));
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

