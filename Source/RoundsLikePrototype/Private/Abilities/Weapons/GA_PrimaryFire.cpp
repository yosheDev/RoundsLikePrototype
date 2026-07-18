// Copyrighted Jacob Jones 2026


#include "Abilities/Weapons/GA_PrimaryFire.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "FPSCharacter.h"

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

    //AFPSCharacter* Character = Cast<AFPSCharacter>(ActorInfo->AvatarActor.Get());

    // Put any custom "can this activate?" logic here.
    // if (Weapon != nullptr && Weapon->CanFire())

    // If GAS determines ability cannot be activated, immediately EndAbility().
    // Calling CommitAbility() here also pays any activation costs setup by GAS.
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Is there an avatar actor?
    if (AActor* Avatar = ActorInfo->AvatarActor.Get())
    {
        // Is avatar a weapon holder?
        if (!Avatar->Implements<UWeaponHolder>())
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        FString RoleString = Avatar->HasAuthority() ? TEXT("Server") : TEXT("Client");

        GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, FString::Printf(TEXT("[%s] Ability Primary Fire"), *RoleString));

        AProjectileWeapon* Weapon = IWeaponHolder::Execute_GetEquippedWeapon(Avatar);

        const UGunplayAttributeSet* Attributes = GetAbilitySystemComponentFromActorInfo()->GetSet<UGunplayAttributeSet>();

        if (Weapon && Weapon->CanFire())
        {
            Weapon->PrimaryFire();
        }
        else 
        {
            GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, FString::Printf(TEXT("[%s] WEAPON IS NULL. Avatar is [%s]"), *RoleString, *Avatar->GetName()));
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_PrimaryFire::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}