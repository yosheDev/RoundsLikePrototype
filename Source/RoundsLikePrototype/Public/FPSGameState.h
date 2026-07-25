// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerState.h"
#include "FPSGameState.generated.h"

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
    WaitingForPlayers,
    AbilityDraft,
    RoundStarting,
    InRound,
    RoundEnd,
    MatchEnd
};

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

protected:

    UFUNCTION()
    void OnRep_MatchPhase();

    UFUNCTION()
    void OnRep_CurrentLoserState();
};
