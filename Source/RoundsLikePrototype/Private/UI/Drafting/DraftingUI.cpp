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
#include "Components/Image.h"
#include "Components/Widget.h"
#include "UI/Drafting/DraftStatButton.h"
#include "Blueprint/SlateBlueprintLibrary.h"

void UDraftingUI::NativeConstruct()
{
    Super::NativeConstruct();

    Bottlecaps.Empty();
    Bottlecaps.Add(Bottlecap1, false);
    Bottlecaps.Add(Bottlecap2, false);
    Bottlecaps.Add(Bottlecap3, false);

    for (int i = 0; i < Bottlecaps.Num(); i++)
    {
        BottlecapStartLocations.Add(FVector2D(0.0f, 0.0f));
        BottlecapEndLocations.Add(FVector2D(0.0f, 0.0f));
        BottlecapTranslationDurations.Add(0.0f);
        BottlecapTimerElapsedTimes.Add(0.0f);
        BottlecapTranslationTimers.Add(FTimerHandle());
    }

    // Ensure the button pointer is valid before binding
    if (SelectDraftButton)
    {
        SelectDraftButton->OnClicked.AddDynamic(this, &UDraftingUI::FinishDraftClick);
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

void UDraftingUI::TranslateBottlecap(uint8 BottlecapID, FVector2D TargetLocSS)
{
    // Get base layout geometry (ignores current render transforms)
    TArray<UImage*> BottlecapImages;
    Bottlecaps.GetKeys(BottlecapImages);

    FGeometry LayoutGeometry = BottlecapImages[BottlecapID]->GetCachedGeometry();

    // Get viewport pos
    FVector2D CurrentPixelPos;
    FVector2D CurrentViewportPos;
    USlateBlueprintLibrary::LocalToViewport(this, LayoutGeometry, FVector2D::ZeroVector, CurrentPixelPos, CurrentViewportPos);

    // Get start and end points for translation.
    BottlecapStartLocations[BottlecapID] = BottlecapImages[BottlecapID]->GetRenderTransform().Translation;
    BottlecapEndLocations[BottlecapID] = TargetLocSS - CurrentViewportPos;
    
    // Calculate distance for bottlecap translation.
    float Distance = FVector2D::Distance(BottlecapStartLocations[BottlecapID], BottlecapEndLocations[BottlecapID]);
    BottlecapTranslationDurations[BottlecapID] = FMath::Max(Distance / FMath::Max(.01f, BottlecapTranslationSpeed), 0.01f);

    BottlecapTimerElapsedTimes[BottlecapID] = 0.0f;

    // Bind input parameters for timer via FTimerDelegate.
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(
        this,
        &UDraftingUI::UpdateBottlecapLocation,
        BottlecapImages[BottlecapID],
        BottlecapID
    );

    // Begin Timer
    GetWorld()->GetTimerManager().SetTimer(
        BottlecapTranslationTimers[BottlecapID],
        TimerDelegate,
        .01f,
        true
    );
}

void UDraftingUI::UpdateBottlecapLocation(UImage* Bottlecap, uint8 BottlecapID)
{
    if (!Bottlecap)
    {
        GetWorld()->GetTimerManager().ClearTimer(BottlecapTranslationTimers[BottlecapID]);
        return;
    }

    // Update elapsed time
    float DeltaSeconds = GetWorld()->GetDeltaSeconds();
    BottlecapTimerElapsedTimes[BottlecapID] += DeltaSeconds;

    // Get alpha
    float Alpha = FMath::Clamp(BottlecapTimerElapsedTimes[BottlecapID] / BottlecapTranslationDurations[BottlecapID], 0.0f, 1.0f);
    Alpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);

    // Movement via Lerp
    FVector2D NewTranslation = FMath::Lerp(BottlecapStartLocations[BottlecapID], BottlecapEndLocations[BottlecapID], Alpha);
    Bottlecap->SetRenderTranslation(NewTranslation);

    // Stop timer at end of translation.
    if (Alpha >= 1.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(BottlecapTranslationTimers[BottlecapID]);
        return;
    }
}

void UDraftingUI::FinishDraftClick()
{
    if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetOwningPlayer()))
    {
        PC->Server_FinishedDraft();
    } 
}

