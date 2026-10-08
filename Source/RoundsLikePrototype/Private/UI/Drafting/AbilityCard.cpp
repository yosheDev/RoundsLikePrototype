// Copyright Jacob Jones 2026


#include "UI/Drafting/AbilityCard.h"
#include "Math/Color.h"
#include "GameplayTagContainer.h"
#include "FPSPlayerState.h"
#include "FPSGameState.h"
#include "FPSPlayerController.h"
#include "UI/Drafting/BottlecapReturnLocation.h"
#include "UI/Drafting/DraftingUI.h"
#include "Abilities/AbilityDefinition.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"

void UAbilityCard::NativeConstruct()
{
    Super::NativeConstruct();

    UUserWidget* ParentWidget = GetTypedOuter<UUserWidget>();
    if (ParentWidget)
    {
        DraftingUI = Cast<UDraftingUI>(ParentWidget);
    }

    if (SelectAbilityButton)
    {
        SelectAbilityButton->OnClicked.AddDynamic(this, &UAbilityCard::SelectAbility);
        SelectAbilityButton->OnHovered.AddDynamic(this, &UAbilityCard::HandleButtonHovered);
        SelectAbilityButton->OnUnhovered.AddDynamic(this, &UAbilityCard::HandleButtonUnhovered);
    }

    // Bind AllocationSucceeded Delegate
    if (AFPSPlayerState* PS = GetOwningPlayer()->GetPlayerState<AFPSPlayerState>())
    {
        PS->OnAllocationSucceeded.AddUObject(
            this,
            &UAbilityCard::HandleAllocationSucceeded);
    }

    // Initialize Render Transforms
    SetRenderTransformPivot(FVector2D(0.5f, 0.5f));

    CurrentScale = NormalScale;
    TargetScale = NormalScale;

    SetRenderScale(FVector2D(CurrentScale));
}

void UAbilityCard::InitializeCard(UAbilityDefinition* NewDataAsset)
{
    AbilityDataAsset = NewDataAsset;

    AbilityName->SetText(AbilityDataAsset->Name);
    AbilityDesc->SetText(AbilityDataAsset->ChangeDescription);
    AbilityFlavor->SetText(AbilityDataAsset->FlavorText);

    Cost = AbilityDataAsset->Cost;
    SetCardColorByRarity(AbilityDataAsset->Rarity);
}

int32 UAbilityCard::GetWidgetID_Implementation()
{
    return WidgetID;
}

void UAbilityCard::SetWidgetID_Implementation(int32 NewID)
{
    WidgetID = NewID;
}

void UAbilityCard::HandleButtonHovered()
{
    if (!DraftingUI) { return; }

    if (!AbilityDataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("DraftUILog: AbilityCard %s has no AbilityDataAsset"), *GetName());
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("DraftUILog: Hover over AbilityCard UI element."));
    if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetOwningPlayer()))
    {
        PC->Server_SetHoveredStat(WidgetID);
        UE_LOG(LogTemp, Log, TEXT("DraftUILog: Call PlayerController::SetHoveredStat ID is [%d]"), WidgetID);
    }
    else
    {
        // As a backup, at least change the local UI to display correctly.
        DraftingUI->StatTitleText->SetText(AbilityDataAsset->Name);
        UE_LOG(LogTemp, Warning, TEXT("DraftUILog: PlayerController was invalid."));
    }
}

void UAbilityCard::HandleButtonUnhovered()
{
    if (!DraftingUI) { return; }

    if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetOwningPlayer()))
    {
        PC->Server_SetHoveredStat(INDEX_NONE);
        UE_LOG(LogTemp, Log, TEXT("DraftUILog: Call PlayerController::SetHoveredStat ID is [%d]"), WidgetID);
    }
    else
    {
        // As a backup, at least change the local UI to display correctly.
        DraftingUI->StatTitleText->SetText(FText::GetEmpty());
    }
}

void UAbilityCard::SetHoveredScale()
{
    TargetScale = HoverScale;

    if (IdleAnimation)
    {
        StopAnimation(IdleAnimation);
    }

    if (IdleHoveredAnimation)
    {
        PlayAnimation(
            IdleHoveredAnimation,
            0.0f,     // Start time
            0,        // 0 = infinite looping
            EUMGSequencePlayMode::Forward,
            1.0f      // Playback speed
        );
    }
}

void UAbilityCard::SetUnhoveredScale()
{
    TargetScale = NormalScale;

    if (IdleHoveredAnimation)
    {
        StopAnimation(IdleHoveredAnimation);
    }

    if (IdleAnimation)
    {
        PlayAnimation(
            IdleAnimation,
            0.0f,     // Start time
            0,        // 0 = infinite looping
            EUMGSequencePlayMode::Forward,
            1.0f      // Playback speed
        );
    }
}

void UAbilityCard::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    CurrentScale = FMath::FInterpTo(
        CurrentScale,
        TargetScale,
        InDeltaTime,
        HoverInterpSpeed
    );

    FWidgetTransform Transform = RenderTransform;

    Transform.Scale = FVector2D(CurrentScale, CurrentScale);

    SetRenderTransform(Transform);
}

#pragma region Allocation and Grant Ability
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
    for (int32 i = 0; i < Cost; i++)
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
}

void UAbilityCard::HandleAllocationSucceeded(int32 InWidgetID)
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

void UAbilityCard::TryDeallocation()
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
#pragma endregion

