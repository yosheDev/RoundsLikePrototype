// Copyright Jacob Jones 2026


#include "FPSGameState.h"
#include "FPSHudController.h"
#include "Subsystems/MatchInstanceSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerController.h"

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameState, MatchPhase);
    DOREPLIFETIME(AFPSGameState, CurrentAbilityOffers);
}

void AFPSGameState::OnRep_MatchPhase()
{
    HandleMatchPhaseChanged();
}

void AFPSGameState::OnRep_CurrentAbilityOffers()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        AFPSHudController* HUD = PC->GetHUD<AFPSHudController>();

        HUD->RefreshAbilitySelection();
    }
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
    UMatchInstanceSubsystem* MatchSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMatchInstanceSubsystem>();

    FMatchData NewData;

    NewData.MatchPhase = MatchPhase;
    NewData.RoundNumber = RoundNumber;
    NewData.PlayerOne = PlayerOne;
    NewData.PlayerTwo = PlayerTwo;
    NewData.PlayerOneWins = PlayerOneWins;
    NewData.PlayerTwoWins = PlayerTwoWins;

    MatchSubsystem->SetMatchData(NewData);
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
}

AFPSPlayerState* AFPSGameState::GetCurrentLoserState()
{
    return CurrentLoserState;
}
