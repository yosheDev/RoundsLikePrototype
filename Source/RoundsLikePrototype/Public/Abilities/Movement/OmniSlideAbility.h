// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OmniSlideAbility.generated.h"

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API UOmniSlideAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

    UOmniSlideAbility();

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

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
    void ExecuteOmniSlide();

    virtual void ExecuteOmniSlide_Implementation();
};
