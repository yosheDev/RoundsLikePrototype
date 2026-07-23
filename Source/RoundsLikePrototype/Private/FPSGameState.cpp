// Copyright Jacob Jones 2026


#include "FPSGameState.h"
#include "Net/UnrealNetwork.h"

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameState, MatchPhase);
}

void AFPSGameState::OnRep_MatchPhase()
{
	
}