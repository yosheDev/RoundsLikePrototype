// Copyright Jacob Jones 2026


#include "FPSGameMode.h"
#include "FPSPlayerState.h"
#include "FPSPlayerController.h"
#include "FPSCharacter.h"
#include "FPSHudController.h"
#include "FPSGameState.h"
#include "Subsystems/AbilityPoolSubsystem.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Enums/SpawnSide.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Spawns/FPSPlayerStart.h"

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	FPSGameState = Cast<AFPSGameState>(GameState);

	UAbilitySystemGlobals::Get().InitGlobalData();
	bUseSeamlessTravel = true;
}

void AFPSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AFPSPlayerState* PS = NewPlayer->GetPlayerState<AFPSPlayerState>();
	UMatchInstanceSubsystem* MatchSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMatchInstanceSubsystem>();

	if (MatchSubsystem->MatchData.PlayerOneId == INDEX_NONE)
	{
		FPSGameState->PlayerOne = PS;
	}
	else if (MatchSubsystem->MatchData.PlayerTwoId == INDEX_NONE)
	{
		FPSGameState->PlayerTwo = PS;
	}

	FPSGameState->UpdateMatchSubsystem();
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

void AFPSGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	RebindPlayerStates();
}

void AFPSGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	FPSGameState = GetGameState<AFPSGameState>();

	if (!IsValid(FPSGameState))
	{
		UE_LOG(LogTemp, Error, TEXT("MatchLog: Failed to retrieve GameState after seamless travel."));
		return;
	}
	
	FPSGameState->InitializeMatchData();

	RebindPlayerStates();

	FPSGameState->UpdateMatchSubsystem();

	UE_LOG(LogTemp, Log,
		TEXT("MatchLog: FPSGameMode::PostSeamlessTravel() - PlayerOne: [%s] PlayerTwo: [%s]"),
		*GetNameSafe(FPSGameState->PlayerOne),
		*GetNameSafe(FPSGameState->PlayerTwo));

	bHasFinishedTravel = true;

	TryRoundStart();
}

void AFPSGameMode::RebindPlayerStates()
{
	if (!IsValid(FPSGameState))
	{
		FPSGameState = GetGameState<AFPSGameState>();
	}

	if (!IsValid(FPSGameState))
	{
		return;
	}

	UMatchInstanceSubsystem* MatchSubsystem = GetGameInstance()->GetSubsystem<UMatchInstanceSubsystem>();

	if (!MatchSubsystem)
	{
		return;
	}

	const int32 PlayerOneId = MatchSubsystem->MatchData.PlayerOneId;
	const int32 PlayerTwoId = MatchSubsystem->MatchData.PlayerTwoId;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();

		if (!IsValid(PC))
		{
			continue;
		}

		AFPSPlayerState* PS = PC->GetPlayerState<AFPSPlayerState>();

		if (!IsValid(PS))
		{
			continue;
		}

		if (PS->GetPlayerId() == PlayerOneId)
		{
			FPSGameState->PlayerOne = PS;
		}
		else if (PS->GetPlayerId() == PlayerTwoId)
		{
			FPSGameState->PlayerTwo = PS;
		}
	}

	UE_LOG(LogTemp, Log,
		TEXT("MatchLog: Rebound PlayerStates. PlayerOne: %s | PlayerTwo: %s"),
		*GetNameSafe(FPSGameState->PlayerOne),
		*GetNameSafe(FPSGameState->PlayerTwo));
}

void AFPSGameMode::RestartPlayer(AController* NewPlayer)
{
	AActor* StartSpot = FindPlayerStart(NewPlayer);

	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);

	// Attempt to start the round.
	SpawnedPlayers.Add(NewPlayer);
	TryRoundStart();
}

ESpawnSide AFPSGameMode::GetSpawnSide(AController* Player)
{
	AFPSPlayerState* PS = Player->GetPlayerState<AFPSPlayerState>();

	if (!IsValid(FPSGameState))
	{
		FPSGameState = GetGameState<AFPSGameState>();
		FPSGameState->InitializeMatchData();
	}

	if (!FPSGameState)
	{
		return bPlayerOneIsRed ? ESpawnSide::Red : ESpawnSide::Blue;
	}

	if (PS == FPSGameState->PlayerOne)
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

	UE_LOG(LogTemp, Warning,
		TEXT("SetMatchPhase: %s Authority=%d"),
		*UEnum::GetValueAsString(NewPhase),
		HasAuthority());

	if (!FPSGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("FPSGameState is NULL. Please assign a GameState derived from FPSGameState in the GameMode class details panel."));
		return;
	}

	FPSGameState->MatchPhase = NewPhase;
	FPSGameState->HandleMatchPhaseChanged();

	FPSGameState->UpdateMatchSubsystem();

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

			if (FPSGameState)
			{
				FPSGameState->StartKillSlowdown();
			}
			
			LoadNextArena();
			break;

		case EMatchPhase::InRound:
			SetPlayersControlEnabled(true);
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
	//DestroyPawns();
	FPSGameState->EconomyComponent->InitializeBottlecapAllocations();
	GenerateAbilityChoices();
	SetMatchPhase(EMatchPhase::AbilityDraft);
}

void AFPSGameMode::PlayerFinishedDraft(AFPSPlayerController* PC)
{
	if (FPSGameState->MatchPhase == EMatchPhase::AbilityDraft)
	{
		if (PC->GetPlayerState<AFPSPlayerState>() == FPSGameState->CurrentLoserState)
		{
			FPSGameState->PlayerOneRoundWins = 0;
			FPSGameState->PlayerTwoRoundWins = 0;

			SetMatchPhase(EMatchPhase::RoundStarting);
		}
	}
}

void AFPSGameMode::BeginRoundStartCountdown()
{
	/** This timer acts as a countdown */

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
		// TO DO: I also need to make sure the pawns have spawned in that the controllers should possess.
		bHasFinishedTravel = false;
		BeginRoundStartCountdown();
	}
}
void AFPSGameMode::StartRound()
{
	// Players should already be spawned by this point. Countdown has ended. Fight!

	SetMatchPhase(EMatchPhase::InRound);

	FPSGameState->RoundNumber++;
}

void AFPSGameMode::OnPlayerDefeated(APlayerController* Loser)
{
	if (FPSGameState->MatchPhase != EMatchPhase::InRound)
	{
		return;
	}

	AFPSPlayerState* LosingPlayer = Loser->GetPlayerState<AFPSPlayerState>();
	
	UE_LOG(LogTemp, Log,
		TEXT("MatchLog: LosingPlayer: %s | PlayerOne: %s | PlayerTwo: %s"),
		*GetNameSafe(LosingPlayer),
		*GetNameSafe(FPSGameState->PlayerOne),
		*GetNameSafe(FPSGameState->PlayerTwo));

	if (!IsValid(LosingPlayer) ||
		!IsValid(FPSGameState->PlayerOne) ||
		!IsValid(FPSGameState->PlayerTwo))
	{
		UE_LOG(LogTemp, Error, TEXT("MatchLog: Cannot award round win. PlayerState references are invalid."));
		return;
	}

	FPSGameState->StartKillSlowdown();

	#pragma region Check Current Round Best of 3 Series
	UE_LOG(LogTemp, Log, TEXT("MatchLog: FPSGameMode::OnPlayerDefeated() BEFORE Assigning RoundWins: PlayerOneRoundWins: [%d] | PlayerTwoRoundWins: [%d]"), FPSGameState->PlayerOneRoundWins, FPSGameState->PlayerTwoRoundWins);

	// First, check if round ends (if best out of 2 has completed)
	if (LosingPlayer == FPSGameState->PlayerOne)
	{
		UE_LOG(LogTemp, Log, TEXT("MatchLog: FPSGameMode::OnPlayerDefeated() PlayerOne lost, so PlayerTwoRoundWins is incremented by 1."));
		FPSGameState->PlayerTwoRoundWins++;
	}
	else if (LosingPlayer == FPSGameState->PlayerTwo)
	{
		UE_LOG(LogTemp, Log, TEXT("MatchLog: FPSGameMode::OnPlayerDefeated() PlayerTwo lost, so PlayerOneRoundWins is incremented by 1."));
		FPSGameState->PlayerOneRoundWins++;
	}

	UE_LOG(LogTemp, Log, TEXT("MatchLog: FPSGameMode::OnPlayerDefeated() AFTER Assigning RoundWins: PlayerOneRoundWins: [%d] | PlayerTwoRoundWins: [%d]"), FPSGameState->PlayerOneRoundWins, FPSGameState->PlayerTwoRoundWins);
	
	// If the round series is not yet over, immediately call RoundEnd without assigning points or current loser on GameState.
	if (FPSGameState->PlayerOneRoundWins < RoundSeriesWinsNeeded && FPSGameState->PlayerTwoRoundWins < RoundSeriesWinsNeeded)
	{
		SetMatchPhase(EMatchPhase::RoundEnd);
		return;
	}
	#pragma endregion

	#pragma region Round Over, Assign Loser and Award Points to Winner

	// Assign loser.
	FPSGameState->CurrentLoserState = LosingPlayer;

	bool bMatchEnd = false;

	// Award points and determine if match is over.
	if (FPSGameState->CurrentLoserState == FPSGameState->PlayerOne)
	{
		GivePoint(2);

		if (FPSGameState->PlayerTwoWins >= WinsRequired)
		{
			bMatchEnd = true;
			FPSGameState->MatchWinnerState = FPSGameState->PlayerTwo;
		}
	}
	else if (FPSGameState->CurrentLoserState == FPSGameState->PlayerTwo)
	{
		GivePoint(1);

		if (FPSGameState->PlayerOneWins >= WinsRequired)
		{
			bMatchEnd = true;
			FPSGameState->MatchWinnerState = FPSGameState->PlayerOne;
		}
	}

	if (bMatchEnd)
	{
		SetMatchPhase(EMatchPhase::MatchEnd);
	}
	else
	{

		SetMatchPhase(EMatchPhase::RoundEnd);
	}
	#pragma endregion
}

void AFPSGameMode::EndMatch()
{
	UE_LOG(LogTemp, Log, TEXT("Match has ended! Winner is [%s]"), *GetNameSafe(Cast<AFPSPlayerController>(FPSGameState->MatchWinnerState->GetOwner())->GetPawn()));
}

#pragma region Server Pending Players Ready
void AFPSGameMode::NotifyRoundEndComplete(AFPSPlayerController* PlayerController)
{
	if (ReadyPlayers.Contains(PlayerController))
	{
		return;
	}

	ReadyPlayers.Add(PlayerController);

	/** Advance to AbilityDraft phase once all players are ready. */
	if (ReadyPlayers.Num() == 2)
	{
		ResetReadyPlayers();

		const bool bSeriesComplete = FPSGameState->PlayerOneRoundWins >= RoundSeriesWinsNeeded || FPSGameState->PlayerTwoRoundWins >= RoundSeriesWinsNeeded;

		if (bSeriesComplete)
		{
			PresentMatchState();
		}
		else
		{
			// TO DO EVENTUALLY: Present match state here but for round wins not total wins.
			SetMatchPhase(EMatchPhase::RoundStarting);
		}
		
	}
}

void AFPSGameMode::NotifyMatchStateDisplayComplete(AFPSPlayerController* PlayerController)
{
	if (ReadyPlayers.Contains(PlayerController))
	{
		return;
	}

	ReadyPlayers.Add(PlayerController);

	/** Advance to AbilityDraft phase once all players are ready. */
	if (ReadyPlayers.Num() == 2)
	{
		ResetReadyPlayers();

		const bool bSeriesComplete = FPSGameState->PlayerOneRoundWins >= RoundSeriesWinsNeeded || FPSGameState->PlayerTwoRoundWins >= RoundSeriesWinsNeeded;

		if (bSeriesComplete)
		{
			StartDraft();
		}
		else
		{
			SetMatchPhase(EMatchPhase::RoundStarting);
		}
	}
}
#pragma endregion

#pragma region Presentation

void AFPSGameMode::PresentMatchState()
{
	// Player controllers need to react and then display the state themselves locally.
	FPSGameState->HandlePresentMatchState();
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
		FPSGameState->PlayerOneWins++;
	}
	else
	{
		FPSGameState->PlayerTwoWins++;
	}
}

void AFPSGameMode::GenerateAbilityChoices()
{
	if (!HasAuthority()) { return; }

	UAbilityPoolSubsystem* AbilityPool = GetGameInstance()->GetSubsystem<UAbilityPoolSubsystem>();

	if (!AbilityPool) { return; }

	FAbilityPoolContext Context;

	Context.OwnedAbilities = FPSGameState->CurrentLoserState->GetOwnedAbilityTags();
	Context.OfferCount = 5;

	for (const FGameplayTag& AbilityTag : FPSGameState->CurrentLoserState->AccruedAbilities)
	{
		Context.OwnedAbilities.AddTag(AbilityTag);
	}

	FPSGameState->EconomyComponent->CurrentAbilityOffers = AbilityPool->GetAbilityOffers(Context);

	UE_LOG(LogTemp, Warning, TEXT("Generated %d ability offers on SERVER"), FPSGameState->EconomyComponent->CurrentAbilityOffers.Num());

	for (const FPrimaryAssetId& ID : FPSGameState->EconomyComponent->CurrentAbilityOffers)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Offer: %s"), *ID.ToString());
	}
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

void AFPSGameMode::SetPlayersControlEnabled(bool bEnabled)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get()))
		{
			if (AFPSCharacter* Character = Cast<AFPSCharacter>(PC->GetPawn()))
			{
				Character->SetCanControl(true);
			}
		}
	}
}
#pragma endregion
