// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "VitalityAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API UVitalityAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// Current health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UVitalityAttributeSet, Health);

	// Transient damage value for detracting health.
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UVitalityAttributeSet, Damage);

	// Upper limit for health value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UVitalityAttributeSet, MaxHealth);

	// Regeneration rate for lifesteal health regen.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UVitalityAttributeSet, HealthRegen);

	// Duration window for lifesteal health regen.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData HealthRegenDuration;
	ATTRIBUTE_ACCESSORS(UVitalityAttributeSet, HealthRegenDuration);

	// Scale of the player avatar.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BodySize;
	ATTRIBUTE_ACCESSORS(UVitalityAttributeSet, BodySize);

public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
