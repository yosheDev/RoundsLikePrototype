// Copyright Jacob Jones 2026


#include "FPSGameMode.h"
#include "FPSPlayerState.h"
#include "FPSPlayerController.h"
#include "FPSHudController.h"
#include "FPSGameState.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Enums/SpawnSide.h"
#include "Spawns/FPSPlayerStart.h"

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	FPSGameState = Cast<AFPSGameState>(GameState);

	bUseSeamlessTravel = true;
}

void AFPSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AFPSPlayerState* PS = NewPlayer->GetPlayerState<AFPSPlayerState>();

	if (!PlayerOne)
	{
		PlayerOne = PS;
	}
	else if (!PlayerTwo)
	{
		PlayerTwo = PS;
	}
}

#pragma region Arena Loading / Player Spawning

void AFPSGameMode::LoadNextArena()
{
	// Update PlayersSpawned count (Players will be destroyed across load.)
	SpawnedPlayers.Empty();

	// Determines which ESpawnSide each player will be spawning at in the next arena.
	bPlayerOneIsRed = FMath::RandBool();
	UsedPlayerStarts.Empty();

	if (ArenaMaps.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to travel to next arena. ArenaMaps container is empty in GameMode instance."));
		return;
	}

	const int32 RandomIndex = FMath::RandRange(0, ArenaMaps.Num() - 1);

	FString MapPath = ArenaMaps[RandomIndex].ToSoftObjectPath().GetLongPackageName();

	GetWorld()->ServerTravel(MapPath);
}

void AFPSGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	if (!IsValid(FPSGameState))
	{
		FPSGameState = GetGameState<AFPSGameState>();
	}

	bHasFinishedTravel = true;

	TryRoundStart();
}

void AFPSGameMode::RestartPlayer(AController* NewPlayer)
{
	AActor* StartSpot = FindPlayerStart(NewPlayer);

	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);

	// Disable player input. Input will be renabled upon countdown end.
	if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(NewPlayer))
	{
		PC->DisableInput(PC);
	}

	// Attempt to start the round.
	SpawnedPlayers.Add(NewPlayer);
	TryRoundStart();
}

ESpawnSide AFPSGameMode::GetSpawnSide(AController* Player)
{
	AFPSPlayerState* PS = Player->GetPlayerState<AFPSPlayerState>();

	if (PS == PlayerOne)
	{
		return bPlayerOneIsRed ? ESpawnSide::Red : ESpawnSide::Blue;
	}

	return bPlayerOneIsRed ? ESpawnSide::Blue : ESpawnSide::Red;
}

AActor* AFPSGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	/* Note:
	 *		 Blue and Red are just codenames for this code for preventing players from spawning at player starts near each other.
	 *		 Goal is just to provide a bit of variety to the spawn locations without making anything biased towards a certain player.
	 */

	// Determine which player has "Red" spawns and which player has "Blue" spawns.
	ESpawnSide DesiredSide = GetSpawnSide(Player);
	TArray<AFPSPlayerStart*> AvailableStarts;

	for (AFPSPlayerStart* Start : TActorRange<AFPSPlayerStart>(GetWorld()))
	{
		if (Start->SpawnSide == DesiredSide)
		{
			if (!UsedPlayerStarts.Contains(Start))
			{
				AvailableStarts.Add(Start);
			}
		}
	}

	if (AvailableStarts.Num() > 0)
	{
		AFPSPlayerStart* ChosenStart = AvailableStarts[FMath::RandRange(0, AvailableStarts.Num() - 1)];
		UsedPlayerStarts.Add(ChosenStart);
		return ChosenStart;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

#pragma endregion

void AFPSGameMode::SetMatchPhase(EMatchPhase NewPhase)
{
	FString PhaseString = UEnum::GetValueAsString(NewPhase);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Set Match Phase To: [%s]"), *PhaseString));

	if (!FPSGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("FPSGameState is NULL. Please assign a GameState derived from FPSGameState in the GameMode class details panel."));
		return;
	}

	FPSGameState->MatchPhase = NewPhase;
	FPSGameState->HandleMatchPhaseChanged();

	switch (NewPhase)
	{
		case EMatchPhase::AbilityDraft:
			/* == MATCH REPLICATION NOTE ==
			* Player controllers react locally to this change, and setup their own UI based on FPSGameState->CurrentLoserState.
			* Only losers can select the ability, the winners just get to watch.
			*/
			break;

		case EMatchPhase::RoundStarting:
			/* == MATCH REPLICATION NOTE ==
			 * The ability draft phase has finished. Begin preparing the next round. 
			 * AFPSGameMode::TryRoundStart() is called only after map is loaded and both players have spawned in.
			 */
			LoadNextArena();
			break;

		case EMatchPhase::InRound:
			break;

		case EMatchPhase::RoundEnd:
			/* == MATCH REPLICATION NOTE ==
			 * Player controllers react locally to this change, and begin their animations, ui, etc. for round ending.
			 * Once completed, it calls AFPSGameMode::ServerNotifyRoundEndComplete().
			 * Only once all players have reported complete, will AFPSGameMode::SetMatchPhase(EMatchPhase::AbilityDraft) be called.
			 */
			break;

		case EMatchPhase::MatchEnd:
			EndMatch();
			break;

		default:
			break;
	}
}

void AFPSGameMode::StartDraft()
{	
	DestroyPawns();
	GenerateAbilityChoices();
	SetMatchPhase(EMatchPhase::AbilityDraft);
}

void AFPSGameMode::PlayerFinishedDraft(AFPSPlayerController* PC)
{
	if (PC->GetPlayerState<AFPSPlayerState>() == FPSGameState->CurrentLoserState)
	{
		SetMatchPhase(EMatchPhase::RoundStarting);
	}
}

void AFPSGameMode::BeginRoundStartCountdown()
{
	// TODO LATER WHEN MAKING FOR REAL: Use state machine more. Can put this entire thing just in my state machine.
	// UI Animation. Could bind to delegate here to know when continue? Worry about specs later after draft concept is known.

	//RespawnPlayers(); // Input disabled, just spawning in and they see countdown and arena they are in before match starts at end of countdown.

	GetWorldTimerManager().SetTimer(
		MatchTimerHandle,
		this,
		&AFPSGameMode::StartRound,
		2.0f,
		false
	);
}

void AFPSGameMode::TryRoundStart()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Try Round Start")));
	if (!IsValid(FPSGameState))
	{
		FPSGameState = GetGameState<AFPSGameState>();
	}

	if (!IsValid(FPSGameState))
	{
		return;
	}

	if (FPSGameState->MatchPhase == EMatchPhase::RoundStarting && bHasFinishedTravel && (SpawnedPlayers.Num() >= 2))
	{
		bHasFinishedTravel = false;
		StartRound();
	}
}
void AFPSGameMode::StartRound()
{
	// Players should already be spawned by this point. Countdown has ended. Fight!

	SetMatchPhase(EMatchPhase::InRound);
}

void AFPSGameMode::OnPlayerDefeated(APlayerController* Loser)
{
	if (FPSGameState->MatchPhase == EMatchPhase::InRound)
	{
		FPSGameState->CurrentLoserState = Loser->GetPlayerState<AFPSPlayerState>();

		if (FPSGameState->CurrentLoserState == PlayerOne)
		{
			GivePoint(2);
		}
		else if (FPSGameState->CurrentLoserState == PlayerTwo)
		{
			GivePoint(1);
		}

		SetMatchPhase(EMatchPhase::RoundEnd);
	}
}

void AFPSGameMode::EndMatch()
{

}

#pragma region Server Pending Players Ready

void AFPSGameMode::NotifyRoundEndComplete(AFPSPlayerController* PlayerController)
{
	ReadyPlayers.Add(PlayerController);

	/** Advance to AbilityDraft phase once all players are ready. */
	if (ReadyPlayers.Num() == 2)
	{
		ResetReadyPlayers();
		StartDraft();
	}
}
#pragma endregion

#pragma region Utility

void AFPSGameMode::ResetReadyPlayers()
{
	ReadyPlayers.Empty();
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

void AFPSGameMode::GenerateAbilityChoices()
{

}

void AFPSGameMode::DestroyPawns()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();

		if (IsValid(PC) && IsValid(PC->GetPawn()))
		{
			PC->GetPawn()->Destroy();
		}
	}
}
#pragma endregion
