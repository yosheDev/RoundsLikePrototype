// Copyright Jacob Jones 2026


#include "Components/MatchEconomyComponent.h"
#include "FPSHudController.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerController.h"

UMatchEconomyComponent::UMatchEconomyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UMatchEconomyComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UMatchEconomyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UMatchEconomyComponent, CurrentAbilityOffers);
}

void UMatchEconomyComponent::OnRep_CurrentAbilityOffers()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        AFPSHudController* HUD = PC->GetHUD<AFPSHudController>();

        HUD->RefreshAbilitySelection();
    }
}

void UMatchEconomyComponent::SetBottlecaps(uint8 Amount)
{
	Bottlecaps = Amount;
}

