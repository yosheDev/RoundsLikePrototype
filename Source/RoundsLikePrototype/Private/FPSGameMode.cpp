// Copyright Jacob Jones 2026


#include "FPSGameMode.h"
#include "FPSPlayerState.h"
#include "FPSPlayerController.h"
#include "FPSGameState.h"

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	FPSGameState = Cast<AFPSGameState>(GameState);
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

}

void AFPSGameMode::GenerateAbilityChoices()
{

}

void AFPSGameMode::PlayerFinishedDraft(AFPSPlayerController* PC)
{

}

void AFPSGameMode::StartRound()
{
	FPSGameState->MatchPhase = EMatchPhase::RoundStarting;

	//RespawnPlayers();

	//EnableInput();

	//Countdown();

	FPSGameState->MatchPhase = EMatchPhase::InRound;
}

void AFPSGameMode::OnPlayerDefeated(AController* Winner, AController* Loser)
{

}

void AFPSGameMode::EndRound()
{

}

void AFPSGameMode::EndMatch()
{

}
