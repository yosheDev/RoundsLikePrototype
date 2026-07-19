// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BulletExecution.generated.h"

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API UBulletExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UBulletExecution();

	//Overridden function. Executes when a Gameplay Effect is applied. Fetches captured attribute set values.
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};