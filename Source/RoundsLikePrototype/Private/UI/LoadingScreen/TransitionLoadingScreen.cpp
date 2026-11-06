// Copyright Jacob Jones 2026


#include "UI/LoadingScreen/TransitionLoadingScreen.h"
#include "UI/LoadingScreen/TransitionSticker.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "TimerManager.h"

void UTransitionLoadingScreen::NativeConstruct()
{
    Super::NativeConstruct();
}

void UTransitionLoadingScreen::NativeDestruct()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(StickerTimerHandle);
	}

	Super::NativeDestruct();
}

void UTransitionLoadingScreen::StartIntro_Implementation()
{
	if (!IsValid(MainCanvas))
	{
		return;
	}

	switch (CurrentLoadingType)
	{
	case EMatchLoadingScreenType::ArenaTravel:
		StartArenaTravelIntro();
		break;

	case EMatchLoadingScreenType::Connecting:
		StartConnectingIntro();
		break;

	case EMatchLoadingScreenType::ReturningToMenu:
		StartReturningToMenuIntro();
		break;

	default:
		OnIntroFinished();
		break;
	}
}

void UTransitionLoadingScreen::StartOutro_Implementation()
{
	switch (CurrentLoadingType)
	{
	case EMatchLoadingScreenType::ArenaTravel:
		StartArenaTravelOutro();
		break;

	case EMatchLoadingScreenType::Connecting:
		StartConnectingOutro();
		break;

	case EMatchLoadingScreenType::ReturningToMenu:
		StartReturningToMenuOutro();
		break;

	default:
		OnOutroFinished();
		break;
	}
}

#pragma region Arena Load Transition
void UTransitionLoadingScreen::StartArenaTravelIntro()
{
	// Defer setup until next tick so that geometry for widget is calculated.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UTransitionLoadingScreen::SetupStickerIntro);
}

void UTransitionLoadingScreen::StartArenaTravelOutro()
{
	SetupStickerOutro();
}
#pragma endregion

#pragma region Menu Travel Transition
void UTransitionLoadingScreen::StartReturningToMenuIntro()
{
	// Defer setup until next tick so that geometry for widget is calculated.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UTransitionLoadingScreen::SetupStickerIntro);
}

void UTransitionLoadingScreen::StartReturningToMenuOutro()
{
	SetupStickerOutro();
}
#pragma endregion

#pragma region Connecting Transition
void UTransitionLoadingScreen::StartConnectingIntro()
{
	// Defer setup until next tick so that geometry for widget is calculated.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UTransitionLoadingScreen::SetupStickerIntro);
}

void UTransitionLoadingScreen::StartConnectingOutro()
{
	SetupStickerOutro();
}
#pragma endregion

void UTransitionLoadingScreen::OnIntroFinished_Implementation()
{
	// Only acknowledge the current Intro once.
	if (bHasAcknowledgedCurrentPhase || CurrentLoadingPhase != EMatchLoadingScreenPhase::Intro)
	{
		return;
	}

	bHasAcknowledgedCurrentPhase = true;

	// Called in BP.
	//NotifyServerLoadingAnimationFinished(EMatchLoadingScreenPhase::Intro);
}

void UTransitionLoadingScreen::OnOutroFinished_Implementation()
{
	// Only acknowledge the current Outro once.
	if (bHasAcknowledgedCurrentPhase || CurrentLoadingPhase != EMatchLoadingScreenPhase::Outro)
	{
		return;
	}

	bHasAcknowledgedCurrentPhase = true;

	// Called in BP.
	//NotifyServerLoadingAnimationFinished(EMatchLoadingScreenPhase::Outro);
}

#pragma region Sticker Functions
void UTransitionLoadingScreen::SetupStickerIntro()
{
	if (!IsValid(MainCanvas) || !IsValid(StickerWidgetClass))
	{
		return;
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(StickerTimerHandle);
	}
	
	MainCanvas->ClearChildren();
	SpawnedStickers.Empty();
	StickerPlacements.Empty();

	NextStickerIndex = 0;

	const FVector2D CanvasSize = MainCanvas->GetCachedGeometry().GetLocalSize();
	if (CanvasSize.IsNearlyZero())
	{
		return;
	}
	const int32 Columns = FMath::Max(1, GridColumns);
	const int32 Rows = FMath::Max(1, GridRows);

	const FVector2D CellSize(CanvasSize.X / Columns, CanvasSize.Y / Rows);
	
	// Predetermine sticker placements.
	for (int32 Y = 0; Y < Rows; ++Y)
	{
		for (int32 X = 0; X < Columns; ++X)
		{
			FStickerPlacement Placement;

			// Position the sticker at the center of its grid cell.
			Placement.Position = FVector2D(
				(X + 0.5f) * CellSize.X,
				(Y + 0.5f) * CellSize.Y
			);

			// Oversize stickers to create overlap and avoid gaps.
			const float Scale = FMath::FRandRange(1.4f, 1.65f);

			Placement.Size = CellSize * Scale;
			Placement.Rotation = FMath::FRandRange(-12.0f, 12.0f);

			StickerPlacements.Add(Placement);
		}
	}

	// Randomize the order in which stickers appear.
	for (int32 Index = StickerPlacements.Num() - 1; Index > 0; --Index)
	{
		const int32 RandomIndex = FMath::RandRange(0, Index);
		StickerPlacements.Swap(Index, RandomIndex);
	}

	UE_LOG(LogTemp, Log, TEXT("TransitionLog: StartIntro() Beginning to spawn stickers."));

	SpawnNextSticker();

	if (StickerPlacements.Num() > 1)
	{
		GetWorld()->GetTimerManager().SetTimer(
			StickerTimerHandle,
			this,
			&UTransitionLoadingScreen::SpawnNextSticker,
			StickerSpawnInterval,
			true
		);
	}
}

void UTransitionLoadingScreen::SpawnNextSticker()
{
	if (!StickerPlacements.IsValidIndex(NextStickerIndex))
	{
		GetWorld()->GetTimerManager().ClearTimer(StickerTimerHandle);
		return;
	}

	UTransitionSticker* NewSticker = CreateWidget<UTransitionSticker>(
		GetOwningPlayer(),
		StickerWidgetClass
	);

	if (!IsValid(NewSticker))
	{
		return;
	}

	const FStickerPlacement& Placement = StickerPlacements[NextStickerIndex];

	UCanvasPanelSlot* CanvasSlot = MainCanvas->AddChildToCanvas(NewSticker);

	if (IsValid(CanvasSlot))
	{
		CanvasSlot->SetPosition(Placement.Position);
		CanvasSlot->SetSize(Placement.Size);
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlot->SetZOrder(NextStickerIndex);

		NewSticker->SetRenderTransformAngle(Placement.Rotation);
	}

	SpawnedStickers.Add(NewSticker);
	++NextStickerIndex;

	if (NextStickerIndex >= StickerPlacements.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(StickerTimerHandle);
		OnIntroFinished();
	}
}

void UTransitionLoadingScreen::SetupStickerOutro()
{
	if (SpawnedStickers.IsEmpty())
	{
		OnOutroFinished();
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(StickerTimerHandle);

	RemoveNextSticker();

	if (!SpawnedStickers.IsEmpty())
	{
		GetWorld()->GetTimerManager().SetTimer(
			StickerTimerHandle,
			this,
			&UTransitionLoadingScreen::RemoveNextSticker,
			StickerSpawnInterval,
			true
		);
	}
}

void UTransitionLoadingScreen::RemoveNextSticker()
{
	if (SpawnedStickers.IsEmpty())
	{
		GetWorld()->GetTimerManager().ClearTimer(StickerTimerHandle);
		OnOutroFinished();
		return;
	}

	UTransitionSticker* Sticker = SpawnedStickers.Last();

	if (IsValid(Sticker))
	{
		Sticker->RemoveFromParent();
	}

	SpawnedStickers.RemoveAt(SpawnedStickers.Num() - 1);

	if (SpawnedStickers.IsEmpty())
	{
		GetWorld()->GetTimerManager().ClearTimer(StickerTimerHandle);
		OnOutroFinished();
	}
}
#pragma endregion