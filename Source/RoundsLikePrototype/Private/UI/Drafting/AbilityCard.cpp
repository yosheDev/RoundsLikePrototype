// Copyright Jacob Jones 2026


#include "UI/Drafting/AbilityCard.h"
#include "GameplayTagContainer.h"
#include "FPSGameState.h"
#include "FPSPlayerState.h"
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
        bIsAllocated = true;

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
                        // TO DO: Have widgets specifically for bottlecap locations instead of using select ability button. That way not the same spot for them all.
                        // Dynamically reposition at construction based on cost.

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

                    if (Locations.Num() != Cost)
                    {
                        return;
                    }

                    FPSGameState->EconomyComponent->AllocateBottlecaps(Cost, WidgetID, Locations);
                }
                else
                {
                    // Nuh uh sfx here
                    return;
                }
            }
        }


        // TO DO: Do not apply this on click. Only apply selected cards when advancing past drafting screen.
        GiveAbilityToPlayer();
    }
    else
    {
        bIsAllocated = false;

        if (UWorld* World = GetWorld())
        {
            AFPSGameState* FPSGameState = World->GetGameState<AFPSGameState>();
            if (FPSGameState)
            {
                FPSGameState->EconomyComponent->DeallocateBottlecaps(WidgetID);
            }
        }
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

