// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "GA_BulletJump.generated.h"

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API UGA_BulletJump : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGA_BulletJump();

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
    void ExecuteBulletJump();

    virtual void ExecuteBulletJump_Implementation();

};
