// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/MovementAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UMovementAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMovementAttributeSet, MaxSpeed);
	DOREPLIFETIME(UMovementAttributeSet, JumpStrength);
	DOREPLIFETIME(UMovementAttributeSet, JumpCount);
	DOREPLIFETIME(UMovementAttributeSet, GravityScale);
	DOREPLIFETIME(UMovementAttributeSet, CrouchedHalfHeight);
}