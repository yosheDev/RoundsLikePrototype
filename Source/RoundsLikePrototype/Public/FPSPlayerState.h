// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "Abilities/AttributeSets/VitalityAttributeSet.h"
#include "Abilities/AttributeSets/MovementAttributeSet.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "FPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
	TObjectPtr<UFPSAbilitySystemComponent> FPSAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UVitalityAttributeSet> VitalityAttributeSet;

	UPROPERTY()
	TObjectPtr<UMovementAttributeSet> MovementAttributeSet;

	UPROPERTY()
	TObjectPtr<UGunplayAttributeSet> GunplayAttributeSet;

public:
	AFPSPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay();

	/** Gameplay cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason);

};
