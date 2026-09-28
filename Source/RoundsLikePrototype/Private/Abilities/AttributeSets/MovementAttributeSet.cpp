// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/MovementAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UMovementAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UMovementAttributeSet, MaxSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(UMovementAttributeSet, JumpStrength);
	DOREPLIFETIME(UMovementAttributeSet, JumpCount);
	DOREPLIFETIME(UMovementAttributeSet, GravityScale);
	DOREPLIFETIME(UMovementAttributeSet, CrouchedHalfHeight);
}

#pragma region OnRep Functions
void UMovementAttributeSet::OnRep_MaxSpeed(const FGameplayAttributeData& OldMaxSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, MaxSpeed, OldMaxSpeed);
    UE_LOG(LogTemp, Warning, TEXT("SpeedTest: [CLIENT] OnRep_MaxSpeed | Old=%f New=%f Base=%f" ), OldMaxSpeed.GetCurrentValue(), MaxSpeed.GetCurrentValue(), MaxSpeed.GetBaseValue());
}
#pragma endregion