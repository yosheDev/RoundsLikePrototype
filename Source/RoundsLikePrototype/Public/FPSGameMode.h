// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSPlayerState.h"
#include "FPSPlayerController.h"
#include "FPSGameState.h"
#include "FPSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	AFPSGameState* FPSGameState;

	UPROPERTY(EditDefaultsOnly)
	uint8 WinsRequired = 5;

	UPROPERTY()
	AFPSPlayerState* PlayerOne;

	UPROPERTY()
	AFPSPlayerState* PlayerTwo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
	uint8 RoundNumber = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
	uint8 PlayerOneWins = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
	uint8 PlayerTwoWins = 0;
	
protected:
	UPROPERTY()
	APlayerController* CurrentLoserController;
public:

	virtual void BeginPlay() override;

	/** State machine execution call for changing the match phase. */
	void SetMatchPhase(EMatchPhase NewPhase);

	void GivePoint(uint8 PlayerID);

	/** Starts the draft phase for abilities. Only loser of previous round picks unless it is the first round, in which both players choose 1 ability. */
	void StartDraft();

	/** Generate the ability pool to choose from. Each player has a seperate pool to pick from. */
	void GenerateAbilityChoices();

	/** Executes when a player has locked in their draft pick. */
	void PlayerFinishedDraft(AFPSPlayerController* PC);

	/** Starts the next round. Loads arena, spawns players, starts game. */
	void StartRound();

	/** Executes when a player dies. First player to die is the loser. */
	void OnPlayerDefeated(APlayerController* Loser);

	/** The round has ended. */
	void EndRound();

	/** The match has ended. */
	void EndMatch();
};
