// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FPSAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilitySpecRecieved, const FGameplayAbilitySpec&);

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UFPSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	FOnAbilitySpecRecieved OnAbilitySpecRecieved;

public:
	UFPSAbilitySystemComponent();
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};
