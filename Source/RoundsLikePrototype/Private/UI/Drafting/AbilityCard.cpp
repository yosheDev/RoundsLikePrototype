// Copyright Jacob Jones 2026


#include "UI/Drafting/AbilityCard.h"
#include "GameplayTagContainer.h"
#include "FPSGameState.h"
#include "FPSPlayerState.h"
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

void UAbilityCard::SelectAbility()
{
    if (!bAlreadySelected)
    {
        bAlreadySelected = true;

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
                        // TO DO: Have widgets specifically for bottlecap locations instead of using select ability button. That way not the same spot for them all.
                        // Dynamically reposition at construction based on cost.

                        // Get target destinations for bottlecaps to slide to.
                        FGeometry CachedGeometry = SelectAbilityButton->GetCachedGeometry();
                        FVector2D LocalLocalCenter = CachedGeometry.GetLocalSize() * 0.5f;
                        FVector2D AbsoluteScreenPosition = CachedGeometry.GetAccumulatedRenderTransform().TransformPoint(LocalLocalCenter);

                        FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(SelectAbilityButton);
                        FVector2D ViewportPosition = USlateBlueprintLibrary::AbsoluteToLocal(ViewportGeometry, AbsoluteScreenPosition);

                        Locations.Add(ViewportPosition);
                    }

                    if (Locations.Num() != Cost)
                    {
                        return;
                    }

                    FPSGameState->EconomyComponent->AllocateBottlecaps(Cost, Locations);
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
}

void UAbilityCard::GiveAbilityToPlayer()
{
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

