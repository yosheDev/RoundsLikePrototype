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

    // Create the Cooldown Spec manually so we can inject the SetByCaller data
    //FGameplayEffectSpecHandle CooldownSpecHandle = MakeOutgoingCooldownEffectSpec(Handle, ActorInfo, ActivationInfo);
    //if (CooldownSpecHandle.IsValid())
    //{
    //    float MyBaseCooldown = 5.0f; // Define your base cooldown for this specific ability here

    //    // Inject the base duration into the spec using the exact tag your MMC expects
    //    CooldownSpecHandle.Data.Get()->SetSetByCallerMagnitude(
    //        FGameplayTag::RequestGameplayTag(FName("Data.CooldownBaseDuration")),
    //        MyBaseCooldown
    //    );

    //    // Apply the customized cooldown spec to the owner
    //    ApplyCooldownEffectSpec(Handle, ActorInfo, ActivationInfo, CooldownSpecHandle);
    //}
    //else
    //{
        bool bCooldownApplied = CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true);
    //}

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