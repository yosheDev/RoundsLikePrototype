// Copyright Jacob Jones 2026


#include "Abilities/Movement/OmniSlideAbility.h"
#include "FPSCharacter.h"
#include "Components/FPSCharacterMovementComponent.h"

UOmniSlideAbility::UOmniSlideAbility()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UOmniSlideAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    UE_LOG(LogTemp, Log, TEXT("SlideLog: Ability activated."));
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AFPSCharacter* Character = Cast<AFPSCharacter>(CurrentActorInfo->AvatarActor.Get());

    // Character is not grounded?
    if (Character->GetCharacterMovement()->IsFalling())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // Already sliding?
    if (Cast<UFPSCharacterMovementComponent>(Character->GetCharacterMovement())->IsSliding())
    {
        UE_LOG(LogTemp, Log, TEXT("SlideLog: Failed: Was already sliding."));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    ExecuteOmniSlide();

    bool bCooldownApplied = CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true);

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UOmniSlideAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOmniSlideAbility::ExecuteOmniSlide_Implementation()
{
    // Overridden in Blueprint
    UE_LOG(LogTemp, Log, TEXT("SlideLog: ExecuteOmniSlide"));
    AFPSCharacter* Character = Cast<AFPSCharacter>(CurrentActorInfo->AvatarActor.Get());

    if (!Character)
    {
        return;
    }
    
    Character->RequestSlide();
}