// Copyright Jacob Jones 2026


#include "Abilities/Movement/GA_BulletJump.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "Weapons/AmmoComponent.h"
#include "FPSCharacter.h"


UGA_BulletJump::UGA_BulletJump()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_BulletJump::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ExecuteBulletJump();

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_BulletJump::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{


    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BulletJump::ExecuteBulletJump_Implementation()
{
    // Use . instead of -> because ActorInfo is now a reference
    AFPSCharacter* Character = Cast<AFPSCharacter>(CurrentActorInfo->AvatarActor.Get());

    if (!Character)
    {
        return;
    }

    const FVector LaunchVelocity = Character->GetActorForwardVector() * 1000.0f + FVector::UpVector * 600.0f;

    Character->LaunchCharacter(
        LaunchVelocity,
        true,  // XY override
        true   // Z override
    );
}