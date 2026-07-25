// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSPlayerController.h"
#include "FPSGameState.h"
#include "Enums/SpawnSide.h"
#include "FPSGameMode.generated.h"

class AFPSPlayerStart;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category = "Default | Match")
	uint8 WinsRequired = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Default | Maps")
	TArray<TSoftObjectPtr<UWorld>> ArenaMaps;

	UPROPERTY()
	AFPSGameState* FPSGameState;

	UPROPERTY()
	TObjectPtr<AFPSPlayerController> PlayerOneController;

	UPROPERTY()
	TObjectPtr<AFPSPlayerController> PlayerTwoController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
	uint8 RoundNumber = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
	uint8 PlayerOneWins = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
	uint8 PlayerTwoWins = 0;
	
protected:
	UPROPERTY()
	TObjectPtr<AFPSPlayerState> PlayerOne;

	UPROPERTY()
	TObjectPtr<AFPSPlayerState> PlayerTwo;

	/** The timer handle that will be used for any countdowns, delays, etc. for match state transitions and flow. */
	UPROPERTY()
	FTimerHandle MatchTimerHandle;

private:
	/** Stores player starts that have already been assigned a player. This prevents players from ever spawning at the same location. */
	UPROPERTY()
	TArray<TObjectPtr<AFPSPlayerStart>> UsedPlayerStarts;

	/** Randomly assigned after loading an arena. This determines which ESpawnSide PlayerOne and PlayerTwo will choose a PlayerStart from. */
	UPROPERTY()
	bool bPlayerOneIsRed;

	/** Keeps track of which player controllers are ready to progress with the match. */
	UPROPERTY()
	TSet<TObjectPtr<AFPSPlayerController>> ReadyPlayers;

public:

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	/** Used for checking if round start countdown should occur yet. */
	UPROPERTY()
	TSet<TObjectPtr<AController>> SpawnedPlayers;

protected:

	/** Determines which FPSPlayerStart a controller should use. Executes as part of Super::RestartPlayer(). */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** Gets the Spawn Side that the player should spawn at for the current arena. */
	ESpawnSide GetSpawnSide(AController* Player);

	/** Callback once ServerTravel() has completely finished. */
	virtual void PostSeamlessTravel() override;

	/** Used for checking if round start countdown should occur yet. True by default for editor testing (being able to load in straight away.) */
	bool bHasFinishedTravel = true;
	
public:

	/** Only succeeds if EMatchPhase is IsStarting, all players have spawned, and seamless travel has completed. */
	void TryRoundStart();

	/** State machine execution call for changing the match phase. */
	void SetMatchPhase(EMatchPhase NewPhase);

	void GivePoint(uint8 PlayerID);

	/** Starts the draft phase for abilities. Only loser of previous round picks unless it is the first round, in which both players choose 1 ability. */
	void StartDraft();

	/** Generate the ability pool to choose from. Each player has a seperate pool to pick from. */
	void GenerateAbilityChoices();

	/** Executes when a player has locked in their draft pick. */
	void PlayerFinishedDraft(AFPSPlayerController* PC);

	/** Loads the next arena and handles Server Travel to it. */
	void LoadNextArena();

	/** Executes after ability draft stage has completed. Counts down the start for the round. */
	void BeginRoundStartCountdown();

	/** Starts the next round. Loads arena, spawns players, starts game. */
	void StartRound();

	/** Executes when a player dies. First player to die is the loser. */
	void OnPlayerDefeated(APlayerController* Loser);

	/** Clients call this once they have completed round end animations/procedures. Ensures client and server are both ready for the draft phase, even on low latency. */
	UFUNCTION()
	void ServerNotifyRoundEndComplete(AFPSPlayerController* PlayerController);

	/** The match has ended. */
	void EndMatch();

	private:

		/** Clears the HashSet being used to store which players are ready to progress. */
		void ResetReadyPlayers();
};
