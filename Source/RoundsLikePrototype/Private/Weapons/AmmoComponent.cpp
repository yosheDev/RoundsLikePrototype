// Copyright Jacob Jones 2026


#include "Weapons/AmmoComponent.h"

#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"       
#include "GameFramework/PlayerController.h"
#include "FPSHudController.h"

UAmmoComponent::UAmmoComponent()
{
    // Tick is only used for incrementing UI display.
	PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.TickInterval = 0.1f;

	SetIsReplicatedByDefault(true);
}

#pragma region Replication Methods
void UAmmoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UAmmoComponent, CurrentAmmo);
    DOREPLIFETIME(UAmmoComponent, EarliestReturnServerTime);
}

void UAmmoComponent::OnRep_CurrentAmmo()
{
    ClientPredictedAmmo = CurrentAmmo;
    UpdateLocalAmmoUI();
}

void UAmmoComponent::OnRep_EarliestReturnServerTime()
{
    if (EarliestReturnServerTime > 0.0f)
    {
        SetComponentTickEnabled(true);
    }
    else
    {
        SetComponentTickEnabled(false);
    }

    UpdateLocalAmmoUI();
}
#pragma endregion

void UAmmoComponent::BeginPlay()
{
	Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        CurrentAmmo = MaxAmmo;
    }
    ClientPredictedAmmo = MaxAmmo;
}

bool UAmmoComponent::TryConsumeAmmo()
{
    if (GetOwner()->HasAuthority())
    {
        // Subtracts from actual ammo count. If successful, calls UpdateLocalAmmoUI().
        return TryConsumeAmmo_Authoratative();
    }
    else
    {
        // Subtracts from local predicted ammo count. If successful, calls UpdateLocalAmmoUI().
        return TryConsumeAmmo_Predicted();
    }
}

bool UAmmoComponent::TryConsumeAmmo_Authoratative()
{
    if (CurrentAmmo <= 0)
    {
        return false;
    }

    CurrentAmmo--;

    const float ReturnTime = GetServerTime() + AmmoReturnDelay;
    PendingReturns.Add(ReturnTime);
    ScheduleNextAmmoReturn();

    UpdateLocalAmmoUI();
           
    return true;
}

bool UAmmoComponent::TryConsumeAmmo_Predicted()
{
    // Server is responsible for the return times. Client only needs to subtract predicted current ammo.
    if (ClientPredictedAmmo <= 0)
    {
        return false;
    }

    ClientPredictedAmmo--;

    UpdateLocalAmmoUI();
    return true;
}

void UAmmoComponent::ScheduleNextAmmoReturn()
{
    if (!GetOwner()->HasAuthority()){ return; }

    if (PendingReturns.Num() == 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(AmmoReturnTimerHandle);
        EarliestReturnServerTime = 0.0f;
        SetComponentTickEnabled(false);
        return;
    }
    
    // Get earliest return time and replicate it for clients. 
    float EarliestReturnTime = PendingReturns[0];
    for (int32 i = 1; i < PendingReturns.Num(); ++i)
    {
        EarliestReturnTime = FMath::Min(EarliestReturnTime, PendingReturns[i]);
    }
    EarliestReturnServerTime = EarliestReturnTime;

    // Start Ammo Return Timer
    const float Delay = FMath::Max(0.0f, EarliestReturnServerTime - GetServerTime());

    GetWorld()->GetTimerManager().SetTimer(
        AmmoReturnTimerHandle,
        this,
        &UAmmoComponent::ProcessAmmoReturns,
        Delay,
        false
    );

    SetComponentTickEnabled(true);
}

void UAmmoComponent::ProcessAmmoReturns()
{
    if (!GetOwner()->HasAuthority()) { return; }

    const float CurrentTime = GetServerTime();

    for (int32 i = PendingReturns.Num() - 1; i >= 0; --i)
    {
        if (PendingReturns[i] <= CurrentTime)
        {
            CurrentAmmo++;

            PendingReturns.RemoveAtSwap(i);
        }
    }

    UpdateLocalAmmoUI();
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

void UAmmoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateLocalAmmoUI();
}

void UAmmoComponent::UpdateLocalAmmoUI()
{
    // Get owning Pawn.
    AActor* Owner = GetOwner()->GetOwner();
    APawn* Pawn = Cast<APawn>(Owner);
    if (!Owner || !Pawn)
    {
        UE_LOG(LogTemp, Log, TEXT("Ammo Pawn was not valid"));
        return;
    }

    // Return if Pawn is not locally controlled.
    if (!Pawn->IsLocallyControlled())
    {
        return;
    }

    // Update UI
    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (PC)
    {
        AFPSHudController* HUD = Cast<AFPSHudController>(PC->GetHUD());
        if (HUD)
        {
            const int32 DisplayAmmo = GetOwner()->HasAuthority() ? CurrentAmmo : ClientPredictedAmmo;
            const float ReturnTimeRemaining = (EarliestReturnServerTime > 0.0f) ? FMath::Max(0.0f, EarliestReturnServerTime - GetServerTime()) : 0.0f;

            if (IsValid(HUD->GetHUDWidget()))
            {
                HUD->GetHUDWidget()->UpdateAmmoSlider(DisplayAmmo, MaxAmmo, ReturnTimeRemaining, AmmoReturnDelay);
            }
        }
    }
}