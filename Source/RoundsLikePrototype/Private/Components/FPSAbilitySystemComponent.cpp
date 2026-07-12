// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FPSAbilitySystemComponent.h"

UFPSAbilitySystemComponent::UFPSAbilitySystemComponent()
{

}

void UFPSAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
    Super::OnGiveAbility(AbilitySpec);

    OnAbilityGranted.Broadcast(AbilitySpec);
}

void UFPSAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

