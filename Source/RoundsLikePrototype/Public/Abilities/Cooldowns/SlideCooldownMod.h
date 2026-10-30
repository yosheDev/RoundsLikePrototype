// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "SlideCooldownMod.generated.h"

UCLASS()
class ROUNDSLIKEPROTOTYPE_API USlideCooldownMod : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	USlideCooldownMod();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	// Defines the attribute we want to capture
	FGameplayEffectAttributeCaptureDefinition CooldownCoefficientDef;
};
