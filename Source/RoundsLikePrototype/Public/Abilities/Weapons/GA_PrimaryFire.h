// Copyrighted Jacob Jones 2026

#pragma once

// Preprocessor directives
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_PrimaryFire.generated.h"

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UGA_PrimaryFire : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

    UGA_PrimaryFire();

    /** Origin point for Weapon to use. Camera location and rotation. May be replaced later on with scene component chid of fp camera.*/
    UPROPERTY()
    FTransform SpawnTransform;

protected:

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;
};
