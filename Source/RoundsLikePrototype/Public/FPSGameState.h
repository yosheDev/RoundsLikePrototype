// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerState.h"
#include "Subsystems/MatchInstanceSubsystem.h"
#include "FPSGameState.generated.h"

UCLASS()
class ROUNDSLIKEPROTOTYPE_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(ReplicatedUsing = OnRep_MatchPhase, BlueprintReadOnly)
    EMatchPhase MatchPhase = EMatchPhase::RoundStarting;

    UFUNCTION()
    void HandleMatchPhaseChanged();

    /** The player state of the previous rounds loser. Used to keep track of who gets to pick an ability for the draft stage. */
    UPROPERTY(ReplicatedUsing = OnRep_CurrentLoserState, BlueprintReadOnly)
    AFPSPlayerState* CurrentLoserState;

    /** The player state of the matches winner upon Match End */
    UPROPERTY(ReplicatedUsing = OnRep_MatchWinnerState, BlueprintReadOnly)
    AFPSPlayerState* MatchWinnerState;

    UFUNCTION()
    void OnRep_MatchWinnerState();

    #pragma region Match Data
    UPROPERTY()
    TObjectPtr<AFPSPlayerState> PlayerOne;

    UPROPERTY()
    TObjectPtr<AFPSPlayerState> PlayerTwo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
    uint8 RoundNumber = 0;

    /** Amount of rounds player 1 has won. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
    uint8 PlayerOneWins = 0;

    /** Amount of rounds player 2 has won. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
    uint8 PlayerTwoWins = 0;

    UFUNCTION(BlueprintCallable, Category = "Match")
    void UpdateMatchSubsystem();

    UFUNCTION(BlueprintCallable, Category = "Match")
    void InitializeMatchData();

    #pragma endregion

protected:

    UFUNCTION()
    void OnRep_MatchPhase();

    UFUNCTION()
    void OnRep_CurrentLoserState();
};
