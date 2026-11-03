// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerState.h"
#include "Subsystems/MatchInstanceSubsystem.h"
#include "Components/MatchEconomyComponent.h"
#include "Curves/CurveFloat.h"
#include "FPSGameState.generated.h"

USTRUCT(BlueprintType)
struct FGamePlayerData
{
    GENERATED_BODY()

    // Alias for the player to go by.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    FName Username = FName();

    // Rounds total won.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    uint8 Points = 0;

    // Amount of wins in current round (between draft phases.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    uint8 RoundWins = 0;
};

UCLASS()
class ROUNDSLIKEPROTOTYPE_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

    AFPSGameState();

    virtual void Tick(float DeltaTime) override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    #pragma region Round Data
    UPROPERTY(ReplicatedUsing = OnRep_MatchPhase, BlueprintReadOnly)
    EMatchPhase MatchPhase = EMatchPhase::RoundStarting;

    UFUNCTION()
    void HandleMatchPhaseChanged();

    /** The player state of the rounds loser. Used to keep track of who gets to pick an ability for the draft stage. */
    UPROPERTY(ReplicatedUsing = OnRep_CurrentLoserState, BlueprintReadOnly)
    AFPSPlayerState* CurrentLoserState;

    /** The player state of the matches winner upon Match End */
    UPROPERTY(ReplicatedUsing = OnRep_MatchWinnerState, BlueprintReadOnly)
    AFPSPlayerState* MatchWinnerState;

    UFUNCTION()
    void OnRep_MatchWinnerState();
    #pragma endregion

    #pragma region Match Data
    UPROPERTY()
    TObjectPtr<AFPSPlayerState> PlayerOne;

    UPROPERTY()
    TObjectPtr<AFPSPlayerState> PlayerTwo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match", Replicated)
    uint8 RoundNumber = 0;

    /** Amount of rounds player 1 has won. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match", Replicated)
    uint8 PlayerOneWins = 0;

    /** Amount of rounds player 2 has won. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match", Replicated)
    uint8 PlayerTwoWins = 0;

    // Number of wins player one has in ONLY the current round.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match", Replicated)
    uint8 PlayerOneRoundWins = 0;

    // Number of wins player two has in ONLY the current round.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match", Replicated)
    uint8 PlayerTwoRoundWins = 0;

    UFUNCTION(BlueprintCallable, Category = "Match")
    void UpdateMatchSubsystem();

    UFUNCTION(BlueprintCallable, Category = "Match")
    void InitializeMatchData();

    UFUNCTION(BlueprintCallable, Category = "Match")
    FGamePlayerData MakePlayerData(uint8 PlayerID) const;
    #pragma endregion

    UPROPERTY()
    TObjectPtr<UMatchEconomyComponent> EconomyComponent;

public:

    AFPSPlayerState* GetCurrentLoserState();

protected:

    UFUNCTION()
    void OnRep_MatchPhase();

    UFUNCTION()
    void OnRep_CurrentLoserState();

#pragma region Presentation
public:

    UFUNCTION()
    void HandlePresentMatchState();
#pragma endregion

#pragma region Kill Slowdown
public:
    // Called when a player gets a kill.
    UFUNCTION(BlueprintCallable, Category = "Kill Effects")
    void StartKillSlowdown();

    UFUNCTION(BlueprintCallable, Category = "Kill Effects")
    void StopKillSlowdown();
protected:

    // Curve controlling global time dilation.
    UPROPERTY(EditDefaultsOnly, Category = "Kill Effects")
    TObjectPtr<UCurveFloat> KillSlowdownCurve;

    // How long the slowdown effect lasts (real-world seconds).
    UPROPERTY(EditDefaultsOnly, Category = "Kill Effects")
    float KillSlowdownDuration = 1.0f;

    // Starts the effect on the server and all clients.
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_StartKillSlowdown();

private:

    bool bKillSlowdownActive = false;

    double KillSlowdownStartTime = 0.0;

    float OriginalTimeDilation = 1.0f;

    void UpdateKillSlowdown();
#pragma endregion
};
