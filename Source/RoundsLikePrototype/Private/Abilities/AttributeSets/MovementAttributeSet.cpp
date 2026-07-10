// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/MovementAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UMovementAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UVitalityAttributeSet, MaxSpeed);
	DOREPLIFETIME(UVitalityAttributeSet, JumpStrength);
	DOREPLIFETIME(UVitalityAttributeSet, JumpCount);
	DOREPLIFETIME(UVitalityAttributeSet, GravityScale);
	DOREPLIFETIME(UVitalityAttributeSet, CrouchedHalfHeight);
}