// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
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
    EMatchPhase MatchPhase;

    UFUNCTION()
    void HandleMatchPhaseChanged();

protected:

    UFUNCTION()
    void OnRep_MatchPhase();
};
