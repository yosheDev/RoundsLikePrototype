// Copyrighted Jacob Jones 2026

// Preprocessor directives
#include "Abilities/Weapons/GA_PrimaryFire.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "FPSCharacter.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"

// Constructor
UGA_PrimaryFire::UGA_PrimaryFire()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Weapon.PrimaryFire"));
}

void UGA_PrimaryFire::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // Is there an avatar actor?
    if (AActor* Avatar = ActorInfo->AvatarActor.Get())
    {
        #pragma region Can This Avatar Shoot?
        // Is avatar a weapon holder?
        if (!Avatar->Implements<UWeaponHolder>())
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        AProjectileWeapon* Weapon = IWeaponHolder::Execute_GetEquippedWeapon(Avatar);

        // Put any custom "can this activate?" logic here.
        if (Weapon == nullptr || !(Weapon->CanFire()))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        // If GAS determines ability cannot be activated, immediately EndAbility().
        // Calling CommitAbility() here also pays any activation costs setup by GAS.
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }
        #pragma endregion

        FString RoleString = Avatar->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
        UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: Ability Primary Fire. Weapon is [%s]"), *RoleString, Weapon ? *Weapon->GetName() : TEXT("NULL"));

        const UGunplayAttributeSet* Attributes = GetAbilitySystemComponentFromActorInfo()->GetSet<UGunplayAttributeSet>();

        #pragma region Activate Primary Fire
        APlayerController* PC = ActorInfo->PlayerController.Get();
        FVector AimLocation = PC->PlayerCameraManager->GetCameraLocation();
        FRotator AimRotation = PC->PlayerCameraManager->GetCameraRotation().Vector().Rotation();
        SpawnTransform = FTransform(AimRotation, AimLocation);

        // Create SpawnTransform of SpawnData here. Weapon unique properties(spread, stats, bullets) will propograte in the AProjectileWeapon.
        FProjectileSpawnData SpawnData;
        SpawnData.SpawnTransform = SpawnTransform;

        Weapon->PrimaryFire(Handle, ActivationInfo, SpawnData);
        #pragma endregion
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_PrimaryFire::EndAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    bool bReplicateEndAbility, 
    bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}