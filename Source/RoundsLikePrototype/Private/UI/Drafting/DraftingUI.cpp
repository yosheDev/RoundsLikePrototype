// Copyright Jacob Jones 2026


#include "UI/Drafting/DraftingUI.h"
#include "FPSGameState.h"
#include "Abilities/AbilityDefinitionHelper.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameMode.h"
#include "FPSPlayerController.h"
#include "Subsystems/AbilityPoolSubsystem.h"
#include "UI/Drafting/AbilityCard.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "UI/Drafting/DraftStatButton.h"

void UDraftingUI::NativeConstruct()
{
    Super::NativeConstruct();

    // Ensure the button pointer is valid before binding
    if (SelectDraftButton)
    {
        SelectDraftButton->OnClicked.AddDynamic(this, &UDraftingUI::HandleMyButtonClick);
    }

    RefreshAbilityCards();
}

void UDraftingUI::RefreshAbilityCards()
{
    int32 CardIndex = 0;
    
    AFPSGameState* FPSGameState = GetWorld()->GetGameState<AFPSGameState>();
    TArray<FPrimaryAssetId> AbilityOffers = FPSGameState->EconomyComponent->CurrentAbilityOffers;

    const double Start = FPlatformTime::Seconds();

    AbilityDefinitions::Load(
        AbilityOffers,
        [this](const TArray<UAbilityDefinition*>& Definitions)
        {
            int32 CardIndex = 0;

            for (int32 i = 0; i < AbilityCards->GetChildrenCount(); i++)
            {
                UAbilityCard* Card = Cast<UAbilityCard>(AbilityCards->GetChildAt(i));

                if (!Card)
                {
                    continue;
                }

                if (!Definitions.IsValidIndex(CardIndex))
                {
                    break;
                }

                Card->AbilityDataAsset = Definitions[CardIndex];

                CardIndex++;
            }
        }
    );

    UE_LOG(LogTemp, Warning, TEXT("AbilityDefinitionHelper::Load() call itself: %.3f ms"), (FPlatformTime::Seconds() - Start) * 1000.0);

    //for (int i = 0; AbilityCards->GetChildrenCount(); i++)
    //{
    //    // Assign data asset to correct card child of the horizontal box
    //    UWidget* Child = AbilityCards->GetChildAt(i);

    //    UAbilityCard* Card = Cast<UAbilityCard>(Child);

    //    if (!Card)
    //    {
    //        continue;
    //    }

    //    if (!AbilityOffers.IsValidIndex(CardIndex))
    //    {
    //        break;
    //    }

    //    AbilityDefinitions::Find(
    //        AbilityOffers[CardIndex],
    //        [Card](UAbilityDefinition* Definition)
    //        {
    //            if (Definition)
    //            {
    //                Card->AbilityDataAsset = Definition;
    //            }
    //        }
    //    );

    //    CardIndex++;
    //}
}

void UDraftingUI::HandleMyButtonClick()
{
    if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetOwningPlayer()))
    {
        PC->Server_FinishedDraft();
    } 
}
