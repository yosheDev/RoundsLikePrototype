// Copyright Jacob Jones 2026


#include "FPSGameMode.h"
#include "FPSPlayerState.h"
#include "FPSPlayerController.h"
#include "FPSHudController.h"
#include "FPSGameState.h"

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	FPSGameState = Cast<AFPSGameState>(GameState);


}

void AFPSGameMode::SetMatchPhase(EMatchPhase NewPhase)
{
	FPSGameState->MatchPhase = NewPhase;

	switch (NewPhase)
	{
		case EMatchPhase::AbilityDraft:
			StartDraft();
			break;
		case EMatchPhase::RoundStarting:
			StartRound();
			break;
		case EMatchPhase::InRound:
			break;
		case EMatchPhase::RoundEnd:
			SetMatchPhase(EMatchPhase::AbilityDraft);
			break;
		case EMatchPhase::MatchEnd:
			EndMatch();
			break;
		default:
			break;
	}
}

void AFPSGameMode::GivePoint(uint8 PlayerID)
{
	if (PlayerID == 1)
	{
		PlayerOneWins++;
	}
	else
	{
		PlayerTwoWins++;
	}
}

void AFPSGameMode::StartDraft()
{
	// Display the draft screen for all player controllers.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It))
		{
			PC->Client_ShowDraftScreen();
		}
	}
}

void AFPSGameMode::GenerateAbilityChoices()
{

}

void AFPSGameMode::PlayerFinishedDraft(AFPSPlayerController* PC)
{

}

void AFPSGameMode::StartRound()
{
	SetMatchPhase(EMatchPhase::RoundStarting);

	//RespawnPlayers();

	//EnableInput();

	//Countdown();

	SetMatchPhase(EMatchPhase::InRound);
}

void AFPSGameMode::OnPlayerDefeated(APlayerController* Loser)
{
	CurrentLoserController = Loser;

	// Do not uncomment until PlayerOne etc is not null.
	//if (Loser->GetPlayerState<AFPSPlayerState>() == PlayerOne)
	//{
	//	// Give Player 2 a point.
	//	GivePoint(2);
	//}
	//else 
	//{
	//	// Give Player 1 a point.
	//	GivePoint(1);
	//}

	EndRound();
}

void AFPSGameMode::EndRound()
{
	SetMatchPhase(EMatchPhase::RoundEnd);
}

void AFPSGameMode::EndMatch()
{

}
