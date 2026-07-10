// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/VitalityAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UVitalityAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UVitalityAttributeSet, Health);
	DOREPLIFETIME(UVitalityAttributeSet, MaxHealth);
	DOREPLIFETIME(UVitalityAttributeSet, HealthRegen);
	DOREPLIFETIME(UVitalityAttributeSet, HealthRegenDuration);
	DOREPLIFETIME(UVitalityAttributeSet, BodySize);
}

