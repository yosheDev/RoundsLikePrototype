// Copyright Jacob Jones 2026


#include "Abilities/Cooldowns/SlideCooldownMod.h"
#include "Abilities/AttributeSets/MovementAttributeSet.h" // Replace with your actual Attribute Set header

USlideCooldownMod::USlideCooldownMod()
{
	// Configure the capture definition for your coefficient attribute
	CooldownCoefficientDef.AttributeToCapture = UMovementAttributeSet::GetSlideCooldownCoefficientAttribute();
	CooldownCoefficientDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	CooldownCoefficientDef.bSnapshot = false;

	// Add it to the array of attributes to capture
	RelevantAttributesToCapture.Add(CooldownCoefficientDef);
}

float USlideCooldownMod::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 1. Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Capture the attribute value
	float Coefficient = 1.0f; // Default fallback multiplier (1.0 = no change)
	GetCapturedAttributeMagnitude(CooldownCoefficientDef, Spec, EvaluationParameters, Coefficient);

	// Get the base duration of the ability
	// Base duration is passed as a SetByCaller magnitude or read it from the Ability itself needs to match same tag as here.
	float BaseDuration = 0.0f;
	Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.CooldownBaseDuration")), false, BaseDuration);

	// Return final duration (Base Duration * Coefficient)
	// Example: Base 10s * 0.8 Coefficient = 8s Cooldown
	return BaseDuration * Coefficient;
}