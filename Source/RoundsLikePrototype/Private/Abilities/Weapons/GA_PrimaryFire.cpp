// Copyrighted Jacob Jones 2026


#include "Abilities/Weapons/GA_PrimaryFire.h"

// Constructor
UGA_PrimaryFire::UGA_PrimaryFire()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Weapon.PrimaryFire"));
}

void UGA_PrimaryFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    // Put any custom "can this activate?" logic here.
    // if (Weapon != nullptr && Weapon->CanFire())

    // If GAS determines ability cannot be activated, immediately EndAbility().
    // Calling CommitAbility() here also pays any activation costs setup by GAS.
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (AActor* Avatar = ActorInfo->AvatarActor.Get())
    {
        FString RoleString =
            Avatar->HasAuthority() ? TEXT("Server") : TEXT("Client");

        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, FString::Printf(TEXT("[%s] Performed Primary Fire"), *RoleString));
    }
    // Weapon->PrimaryFire();

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_PrimaryFire::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}