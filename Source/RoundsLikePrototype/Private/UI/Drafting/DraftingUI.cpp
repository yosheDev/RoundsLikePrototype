// Copyright Jacob Jones 2026


#include "UI/Drafting/DraftingUI.h"
#include "UI/Drafting/AllocationWidgetIDInterface.h"
#include "UI/Drafting/BottlecapReturnLocation.h"
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
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "UI/Drafting/DraftStatButton.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"

void UDraftingUI::NativeConstruct()
{
    Super::NativeConstruct();

    //ForceLayoutPrepass();

    #pragma region Initialize Bottlecap Data
    Bottlecaps.Empty();
    Bottlecaps.Add(Bottlecap1, false);
    //BottlecapDefaultLocations.Add(GetWidgetCenterInViewport(Bottlecap1));
    //BottlecapIsDefaultLocationTaken.Add(true);

    Bottlecaps.Add(Bottlecap2, false); 
    //BottlecapDefaultLocations.Add(GetWidgetCenterInViewport(Bottlecap2));
    //BottlecapIsDefaultLocationTaken.Add(true);

    Bottlecaps.Add(Bottlecap3, false);
    //BottlecapDefaultLocations.Add(GetWidgetCenterInViewport(Bottlecap3));
    //BottlecapIsDefaultLocationTaken.Add(true);

    FTimerHandle BottlecapInitializationTimer;

    GetWorld()->GetTimerManager().SetTimer(
        BottlecapInitializationTimer,
        this,
        &UDraftingUI::InitializeBottlecapDefaultLocations,
        0.1f,
        false
    );
    #pragma endregion

    for (int i = 0; i < Bottlecaps.Num(); i++)
    {
        BottlecapCurrentDefaultSlots.Add(i);
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
    AssignStatWidgetIDs();
}

void UDraftingUI::InitializeBottlecapDefaultLocations()
{
    if (!Bottlecap1 || !Bottlecap2 || !Bottlecap3)
    {
        return;
    }

    BottlecapDefaultLocations.Empty();
    BottlecapIsDefaultLocationTaken.Empty();

    BottlecapDefaultLocations.Add(
        GetWidgetCenterInViewport(Bottlecap1)
    );

    BottlecapIsDefaultLocationTaken.Add(true);

    BottlecapDefaultLocations.Add(
        GetWidgetCenterInViewport(Bottlecap2)
    );

    BottlecapIsDefaultLocationTaken.Add(true);

    BottlecapDefaultLocations.Add(
        GetWidgetCenterInViewport(Bottlecap3)
    );

    BottlecapIsDefaultLocationTaken.Add(true);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Bottlecap slot 0: %s"),
        *BottlecapDefaultLocations[0].ToString()
    );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Bottlecap slot 1: %s"),
        *BottlecapDefaultLocations[1].ToString()
    );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Bottlecap slot 2: %s"),
        *BottlecapDefaultLocations[2].ToString()
    );
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

                // Assign Widget ID safely using the Unreal Engine Execute system
                if (Card && Card->GetClass()->ImplementsInterface(UAllocationWidgetIDInterface::StaticClass()))
                {
                    IAllocationWidgetIDInterface::Execute_SetWidgetID(Card, CardIndex);
                }

                CardIndex++;
            }
        }
    );

    UE_LOG(LogTemp, Warning, TEXT("AbilityDefinitionHelper::Load() call itself: %.3f ms"), (FPlatformTime::Seconds() - Start) * 1000.0);
}

void UDraftingUI::AssignStatWidgetIDs()
{
    int32 StatIndex = 10;

    for (int32 i = 0; i < StatButtons->GetChildrenCount(); i++)
    {
        UDraftStatButton* Button = Cast<UDraftStatButton>(StatButtons->GetChildAt(i));

        if (!Button)
        {
            continue;
        }

        // Assign Widget ID
        if (Button && Button->GetClass()->ImplementsInterface(UAllocationWidgetIDInterface::StaticClass()))
        {
            IAllocationWidgetIDInterface::Execute_SetWidgetID(Button, StatIndex);
        }

        StatIndex++;
    }
}

void UDraftingUI::TranslateBottlecap(uint8 BottlecapID, FBottlecapReturnLocation TargetLocSS, bool bIsDeallocating)
{
    if (bIsDeallocating)
    {

        // Return bottlecap to slot TargetLocSS.SlotIndex
        BottlecapIsDefaultLocationTaken[TargetLocSS.SlotIndex] = true;
        BottlecapCurrentDefaultSlots[BottlecapID] = TargetLocSS.SlotIndex;
    }
    else
    {
        // Free up the bottlecap slot bottlecap was taking up.
        const int32 CurrentSlot = BottlecapCurrentDefaultSlots[BottlecapID];

        if (CurrentSlot != -1)
        {
            BottlecapIsDefaultLocationTaken[CurrentSlot] = false;
            BottlecapCurrentDefaultSlots[BottlecapID] = -1;
        }
    }

    //// Get base layout geometry (ignores current render transforms)
    //TArray<UImage*> BottlecapImages;
    //Bottlecaps.GetKeys(BottlecapImages);

    //FGeometry LayoutGeometry = BottlecapImages[BottlecapID]->GetCachedGeometry();

    //// Get viewport pos
    //FVector2D CurrentPixelPos;
    //FVector2D CurrentViewportPos;
    //USlateBlueprintLibrary::LocalToViewport(this, LayoutGeometry, FVector2D::ZeroVector, CurrentPixelPos, CurrentViewportPos);

    //// Get start and end points for translation.
    //BottlecapStartLocations[BottlecapID] = BottlecapImages[BottlecapID]->GetRenderTransform().Translation;
    //BottlecapEndLocations[BottlecapID] = TargetLocSS - CurrentViewportPos;
    //
    //// Calculate distance for bottlecap translation.
    //float Distance = FVector2D::Distance(BottlecapStartLocations[BottlecapID], BottlecapEndLocations[BottlecapID]);
    //BottlecapTranslationDurations[BottlecapID] = FMath::Max(Distance / FMath::Max(.01f, BottlecapTranslationSpeed), 0.01f);

    //BottlecapTimerElapsedTimes[BottlecapID] = 0.0f;

    //// Bind input parameters for timer via FTimerDelegate.
    //FTimerDelegate TimerDelegate;
    //TimerDelegate.BindUObject(
    //    this,
    //    &UDraftingUI::UpdateBottlecapLocation,
    //    BottlecapImages[BottlecapID],
    //    BottlecapID
    //);

    //// Begin Timer
    //GetWorld()->GetTimerManager().SetTimer(
    //    BottlecapTranslationTimers[BottlecapID],
    //    TimerDelegate,
    //    .01f,
    //    true
    //);

    TArray<UImage*> BottlecapImages;
    Bottlecaps.GetKeys(BottlecapImages);

    if (!BottlecapImages.IsValidIndex(BottlecapID))
    {
        UE_LOG(LogTemp, Error, TEXT("TranslateBottlecap: Invalid BottlecapID %d"), BottlecapID);
        return;
    }

    UImage* Bottlecap = BottlecapImages[BottlecapID];

    if (!Bottlecap)
    {
        return;
    }

    const FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
    const FVector2D TargetAbsolute = ViewportGeometry.LocalToAbsolute(TargetLocSS.Location);

    const FGeometry BottlecapGeometry = Bottlecap->GetCachedGeometry();
    const FVector2D BottlecapCenterLocal = BottlecapGeometry.GetLocalSize() * 0.5f;
    const FVector2D BottlecapCenterAbsolute = BottlecapGeometry.LocalToAbsolute(BottlecapCenterLocal);

    const FSlateRenderTransform AccumulatedTransform = BottlecapGeometry.GetAccumulatedRenderTransform();

    FVector2D LocalDelta = TargetAbsolute - BottlecapCenterAbsolute; // Not local yet, absolute
    LocalDelta = AccumulatedTransform.Inverse().TransformVector(LocalDelta); // Local now

    const FVector2D StartTranslation = Bottlecap->GetRenderTransform().Translation;
    const FVector2D EndTranslation = StartTranslation + LocalDelta;

    BottlecapStartLocations[BottlecapID] = StartTranslation;
    BottlecapEndLocations[BottlecapID] = EndTranslation;

    const float Distance = FVector2D::Distance(StartTranslation, EndTranslation);

    const float Speed = FMath::Max(BottlecapTranslationSpeed, 0.01f);

    BottlecapTranslationDurations[BottlecapID] = FMath::Max(Distance / Speed, 0.01f);

    BottlecapTimerElapsedTimes[BottlecapID] = 0.0f;

    FTimerDelegate TimerDelegate;

    TimerDelegate.BindUObject(
        this,
        &UDraftingUI::UpdateBottlecapLocation,
        Bottlecap,
        BottlecapID);

    GetWorld()->GetTimerManager().SetTimer(
        BottlecapTranslationTimers[BottlecapID],
        TimerDelegate,
        0.01f,
        true);
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

const TArray<FBottlecapReturnLocation> UDraftingUI::GetBottlecapReturnLocations(uint8 Amount)
{
    TArray<FBottlecapReturnLocation> ReturnLocations;
    
    // If return location has bottlecap allocated, add to array.
    for (int i = 0; i < BottlecapDefaultLocations.Num(); i++)
    {
        if (!BottlecapIsDefaultLocationTaken[i])
        {
            FBottlecapReturnLocation NewReturnLocation;
            NewReturnLocation.SlotIndex = i;
            NewReturnLocation.Location = BottlecapDefaultLocations[i];
            ReturnLocations.Add(NewReturnLocation);
            BottlecapIsDefaultLocationTaken[i] = true;
        }

        // Do we have enough?
        if (ReturnLocations.Num() >= Amount)
        {
            break;
        }
    }

    // Trim excess
    //ReturnLocations.SetNum(Amount);

    return ReturnLocations;
}

FVector2D UDraftingUI::GetWidgetCenterInCanvas(UWidget* Widget) const
{
    if (!Widget || !MainCanvas)
    {
        return FVector2D::ZeroVector;
    }

    const FGeometry WidgetGeometry = Widget->GetCachedGeometry();
    const FGeometry CanvasGeometry = MainCanvas->GetCachedGeometry();

    // Widget local center -> absolute
    const FVector2D WidgetCenterLocal = WidgetGeometry.GetLocalSize() * 0.5f;

    const FVector2D WidgetCenterAbsolute = WidgetGeometry.LocalToAbsolute(WidgetCenterLocal);

    // Absolute -> Canvas local
    return CanvasGeometry.AbsoluteToLocal(WidgetCenterAbsolute);
}

FVector2D UDraftingUI::GetWidgetCenterInViewport(UWidget* Widget) const
{
    //FGeometry CachedGeometry = Widget->GetCachedGeometry();
    //// Get absolute center.
    //FVector2D AbsoluteCenter = CachedGeometry.GetLocalSize() * 0.5f;
    //AbsoluteCenter = CachedGeometry.LocalToAbsolute(AbsoluteCenter);
    //// Get absolute center relative to viewport.
    //FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);
    //FVector2D ViewportCenter = ViewportGeometry.AbsoluteToLocal(AbsoluteCenter);

    //return ViewportCenter;
    if (!Widget)
    {
        return FVector2D::ZeroVector;
    }

    const FGeometry& Geometry = Widget->GetTickSpaceGeometry();

    const FVector2D LocalCenter =
        Geometry.GetLocalSize() * 0.5f;

    const FVector2D AbsoluteCenter =
        Geometry.LocalToAbsolute(LocalCenter);

    FVector2D PixelPosition;
    FVector2D ViewportPosition;

    USlateBlueprintLibrary::AbsoluteToViewport(
        this,
        AbsoluteCenter,
        PixelPosition,
        ViewportPosition);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("%s | Size=%s | Absolute=%s | Viewport=%s"),
        *Widget->GetName(),
        *Geometry.GetLocalSize().ToString(),
        *AbsoluteCenter.ToString(),
        *ViewportPosition.ToString());

    return ViewportPosition;
}

FVector2D UDraftingUI::GetViewportPositionInCanvas(FVector2D ViewportPosition) const
{
    if (!MainCanvas)
    {
        return FVector2D::ZeroVector;
    }

    const FGeometry CanvasGeometry = MainCanvas->GetCachedGeometry();

    const FGeometry ViewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);

    // Viewport -> absolute
    const FVector2D AbsolutePosition = ViewportGeometry.LocalToAbsolute(ViewportPosition);

    // Absolute -> Canvas local
    return CanvasGeometry.AbsoluteToLocal(AbsolutePosition);
}

void UDraftingUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (BottlecapIsDefaultLocationTaken.Num() >= 3)
    {
        BottlecapSlot1Debug->SetText(FText::FromString(BottlecapIsDefaultLocationTaken[0] ? TEXT("true") : TEXT("false")));
        BottlecapSlot2Debug->SetText(FText::FromString(BottlecapIsDefaultLocationTaken[1] ? TEXT("true") : TEXT("false")));
        BottlecapSlot3Debug->SetText(FText::FromString(BottlecapIsDefaultLocationTaken[2] ? TEXT("true") : TEXT("false")));
    }
}

