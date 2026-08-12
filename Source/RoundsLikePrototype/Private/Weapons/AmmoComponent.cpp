// Copyright Jacob Jones 2026


#include "Weapons/AmmoComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "TimerManager.h"

UAmmoComponent::UAmmoComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UAmmoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAmmoComponent, CurrentAmmo);
}

void UAmmoComponent::OnRep_CurrentAmmo()
{
    ClientPredictedAmmo = CurrentAmmo;
}

// Called when the game starts
void UAmmoComponent::BeginPlay()
{
	Super::BeginPlay();

    CurrentAmmo = MaxAmmo;
    ClientPredictedAmmo = MaxAmmo;
}

bool UAmmoComponent::TryConsumeAmmo()
{
    if (!GetOwner()->HasAuthority())
    {
        // Server is responsible for the return times. Client only needs to subtract current ammo.
        if (ClientPredictedAmmo <= 0)
        {
            return false;
        }
        
        ClientPredictedAmmo--;
        return true;
    }

    if (CurrentAmmo <= 0)
    {
        return false;
    }

    CurrentAmmo--;

    const float ReturnTime = GetServerTime() + AmmoReturnDelay; // Get ServerTime?

    PendingReturns.Add(ReturnTime);

    ScheduleNextAmmoReturn();

    return true;
}

void UAmmoComponent::ScheduleNextAmmoReturn()
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    if (PendingReturns.Num() == 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(AmmoReturnTimerHandle);
        return;
    }

    float EarliestReturnTime = PendingReturns[0];

    for (int32 i = 1; i < PendingReturns.Num(); ++i)
    {
        EarliestReturnTime = FMath::Min(EarliestReturnTime, PendingReturns[i]);
    }

    const float Delay = FMath::Max(0.0f, EarliestReturnTime - GetServerTime());

    GetWorld()->GetTimerManager().SetTimer(
        AmmoReturnTimerHandle,
        this,
        &UAmmoComponent::ProcessAmmoReturns,
        Delay,
        false
    );
}

void UAmmoComponent::ProcessAmmoReturns()
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    const float CurrentTime = GetServerTime();

    for (int32 i = PendingReturns.Num() - 1; i >= 0; --i)
    {
        if (PendingReturns[i] <= CurrentTime)
        {
            ++CurrentAmmo;

            PendingReturns.RemoveAtSwap(i);
        }
    }

    ScheduleNextAmmoReturn();
}

float UAmmoComponent::GetServerTime() const
{
    if (UWorld* World = GetWorld())
    {
        return World->GetGameState()
            ? World->GetGameState()->GetServerWorldTimeSeconds()
            : World->GetTimeSeconds();
    }

    return 0.0f;
}

bool UAmmoComponent::HasAmmo() const
{
    return CurrentAmmo > 0;
}