// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Preprocessor Directives
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "VitalityAttributeSet.generated.h"

// Forward Declarations
class AFPSCharacter;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// Delegate used to immediately let clients display health changes. Later, they will be conciled through GAS replication.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageTaken, float);

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API UVitalityAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	UVitalityAttributeSet();

	// Current health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UVitalityAttributeSet, Health);

	// Transient damage value for detracting health.
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UVitalityAttributeSet, Damage);

	// Upper limit for health value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
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

#pragma region OnRep Functions
protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldHealth);
#pragma endregion
public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
