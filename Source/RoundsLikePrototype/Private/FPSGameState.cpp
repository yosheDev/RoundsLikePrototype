// Copyright Jacob Jones 2026


#include "FPSGameState.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerController.h"

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
