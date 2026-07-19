// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/VitalityAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UVitalityAttributeSet::UVitalityAttributeSet()
{
	InitHealth(10000.0f);
	InitMaxHealth(10000.0f);
}

void UVitalityAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UVitalityAttributeSet, Health);
	DOREPLIFETIME(UVitalityAttributeSet, MaxHealth);
	DOREPLIFETIME(UVitalityAttributeSet, HealthRegen);
	DOREPLIFETIME(UVitalityAttributeSet, HealthRegenDuration);
	DOREPLIFETIME(UVitalityAttributeSet, BodySize);
}

void UVitalityAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("PreChange: Attribute '%s'"), *Attribute.AttributeName);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	Super::PreAttributeChange(Attribute, NewValue);
}

void UVitalityAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float AppliedDamage = GetDamage();
		const float NewHealth = FMath::Clamp(GetHealth() - AppliedDamage, 0.0f, GetMaxHealth());

		SetHealth(NewHealth);
		SetDamage(0.f);
	}
}

