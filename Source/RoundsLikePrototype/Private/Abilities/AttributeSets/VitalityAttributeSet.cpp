// Fill out your copyright notice in the Description page of Project Settings.

// Preprocessor Directives
#include "Abilities/AttributeSets/VitalityAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "FPSCharacter.h"

UVitalityAttributeSet::UVitalityAttributeSet()
{
	InitHealth(10000.0f);
	InitMaxHealth(10000.0f);
}

void UVitalityAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UVitalityAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVitalityAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(UVitalityAttributeSet, HealthRegen);
	DOREPLIFETIME(UVitalityAttributeSet, HealthRegenDuration);
	DOREPLIFETIME(UVitalityAttributeSet, BodySize);
}

#pragma region OnRep Functions
void UVitalityAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVitalityAttributeSet, Health, OldHealth);
}

void UVitalityAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVitalityAttributeSet, MaxHealth, OldMaxHealth);
}
#pragma endregion

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

		// Broadcast a damage event so that clients can see health bar updating smoothly instead of waiting for OnRep stuff.
		if (AFPSCharacter* Character = Cast<AFPSCharacter>(Data.Target.AbilityActorInfo->AvatarActor.Get()))
		{
			Character->MulticastDamageTaken(AppliedDamage);
		}
	}
}

