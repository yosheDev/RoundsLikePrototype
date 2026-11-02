// Copyright Jacob Jones 2026


#include "FPSGameState.h"
#include "FPSHudController.h"
#include "Subsystems/MatchInstanceSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerController.h"
#include "Components/MatchEconomyComponent.h"

AFPSGameState::AFPSGameState()
{
    EconomyComponent = CreateDefaultSubobject<UMatchEconomyComponent>(TEXT("EconomyComponent"));
}

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameState, MatchPhase);
}

void AFPSGameState::OnRep_MatchPhase()
{
    HandleMatchPhaseChanged();
}

void AFPSGameState::HandleMatchPhaseChanged()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It))
        {
            PC->OnMatchPhaseChanged(MatchPhase);
        }
    }
}

void AFPSGameState::OnRep_CurrentLoserState()
{

}

void AFPSGameState::OnRep_MatchWinnerState()
{

}

void AFPSGameState::UpdateMatchSubsystem()
{
    if (!HasAuthority())
    {
        return;
    }

    UMatchInstanceSubsystem* MatchSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMatchInstanceSubsystem>();

    FMatchData NewData;

    NewData.MatchPhase = MatchPhase;
    NewData.RoundNumber = RoundNumber;
    NewData.PlayerOne = PlayerOne;
    NewData.PlayerTwo = PlayerTwo;
    NewData.PlayerOneWins = PlayerOneWins;
    NewData.PlayerTwoWins = PlayerTwoWins;
    NewData.PlayerOneRoundWins = PlayerOneRoundWins;
    NewData.PlayerTwoRoundWins = PlayerTwoRoundWins;
    NewData.PlayerOneId = IsValid(PlayerOne) ? PlayerOne->GetPlayerId() : MatchSubsystem->MatchData.PlayerOneId;
    NewData.PlayerTwoId = IsValid(PlayerTwo) ? PlayerTwo->GetPlayerId() : MatchSubsystem->MatchData.PlayerTwoId;

    MatchSubsystem->SetMatchData(NewData);
    UE_LOG(LogTemp, Log, TEXT("MatchLog: MatchSubsystem is updated."));
}

void AFPSGameState::InitializeMatchData()
{
    UMatchInstanceSubsystem* MatchSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMatchInstanceSubsystem>();

    FMatchData NewData = MatchSubsystem->GetMatchData();

    MatchPhase = NewData.MatchPhase;
    RoundNumber = NewData.RoundNumber;
    PlayerOne = NewData.PlayerOne;
    PlayerTwo = NewData.PlayerTwo;
    PlayerOneWins = NewData.PlayerOneWins;
    PlayerTwoWins = NewData.PlayerTwoWins;
    PlayerOneRoundWins = NewData.PlayerOneRoundWins;
    PlayerTwoRoundWins = NewData.PlayerTwoRoundWins;

    UE_LOG(LogTemp, Log, TEXT("MatchLog: Initialized MatchData on GameState from Subsystem Data."));
}

AFPSPlayerState* AFPSGameState::GetCurrentLoserState()
{
    return CurrentLoserState;
}
