// Copyright Jacob Jones 2026


#include "UI/Presentation/MatchStateDisplayWidget.h"
#include "FPSPlayerController.h"

void UMatchStateDisplayWidget::UpdateMatchStateDisplay_Implementation(FGamePlayerData PlayerOneData, FGamePlayerData PlayerTwoData, uint8 CurrentLoserID)
{
	// Functionality in BP cause for widgets with specigic timing and animations that is just easier to work with.
	// Get winning player, set theirs to be -1 point cause will do display update to display it updating live itll look cooler.
}

void UMatchStateDisplayWidget::EndDisplay()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		// Tell the Player Controller to notify the server GameMode that this player has finished displaying the Match State.
		Cast<AFPSPlayerController>(PC)->Server_NotifyMatchStateDisplayComplete();
	}

	SetVisibility(ESlateVisibility::Collapsed);
}