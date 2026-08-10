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

    NextFireTime = 0.0f;
    LastFireTime = 0.0f;
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
        #pragma region Weapon Validation

        if (!Avatar->Implements<UWeaponHolder>())
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        // This is suddenly failing on clients. Didnt even change anything wtf. Weapon is null.
        AProjectileWeapon* Weapon = IWeaponHolder::Execute_GetEquippedWeapon(Avatar);
        if (Weapon == nullptr || !(Weapon->CanFire()))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        FString RoleString = ActorInfo->AvatarActor.Get()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
        UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: Has passed weapon check"), *RoleString);

        #pragma endregion

        // This allows users to shoot fast while spam clicking, but not faster than bare minimum .1f.
        double CurrentTime = GetWorld()->GetTimeSeconds();
        if (CurrentTime < (LastFireTime + .1f))
        {
            EndAbility(
                CurrentSpecHandle,
                CurrentActorInfo,
                CurrentActivationInfo,
                true,
                false);

            return;
        }

        // If GAS determines ability cannot be activated, immediately EndAbility().
        // Calling CommitAbility() here also pays any activation costs setup by GAS.
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        Attributes = GetAbilitySystemComponentFromActorInfo()->GetSet<UGunplayAttributeSet>();

        FireShot();

        ScheduleNextShot();

        //FString 
            RoleString = Avatar->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
        UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: End of Primary Fire Ability. Weapon is [%s]"), *RoleString, Weapon ? *Weapon->GetName() : TEXT("NULL"));
    }

    //EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_PrimaryFire::EndAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    bool bReplicateEndAbility, 
    bool bWasCancelled)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(FireTimerHandle);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_PrimaryFire::ScheduleNextShot()
{
    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    World->GetTimerManager().SetTimer(
        FireTimerHandle,
        this,
        &UGA_PrimaryFire::FireShot,
        GetFireInterval(),
        false);
}

void UGA_PrimaryFire::FireShot()
{
    UE_LOG(LogTemp, Log, TEXT("FireLog: FireShot()"));
    if (!CanFire())
    {
        EndAbility(
            CurrentSpecHandle,
            CurrentActorInfo,
            CurrentActivationInfo,
            true,
            false);

        return;
    }

    LastFireTime = GetWorld()->GetTimeSeconds();
    NextFireTime = NextFireTime + GetFireInterval();

    #pragma region Activate Primary Fire
    APlayerController* PC = CurrentActorInfo->PlayerController.Get();
    FVector AimLocation = PC->PlayerCameraManager->GetCameraLocation();
    FRotator AimRotation = PC->PlayerCameraManager->GetCameraRotation().Vector().Rotation();
    AimRotation.Pitch += Attributes->GetBulletArc();
    SpawnTransform = FTransform(AimRotation, AimLocation);

    // Create SpawnTransform of SpawnData here. Weapon unique properties(spread, stats, bullets) will propograte in the AProjectileWeapon.
    FProjectileSpawnData SpawnData;
    SpawnData.SpawnTransform = SpawnTransform;

    AActor* Avatar = CurrentActorInfo->AvatarActor.Get();
    if (!IsValid(Avatar))
    {
        return;
    }

    AProjectileWeapon* Weapon = IWeaponHolder::Execute_GetEquippedWeapon(Avatar);

    Weapon->PrimaryFire(CurrentSpecHandle, CurrentActivationInfo, SpawnData);
    #pragma endregion

    ScheduleNextShot();
}

float UGA_PrimaryFire::GetFireInterval() const
{
    float FireRate = Attributes->GetAutoFireRate();

    if (FireRate <= 0.f)
    {
        return 5.0f;
    }

    return 1.0f / FireRate;
}

bool UGA_PrimaryFire::CanFire() const
{
    double CurrentTime = GetWorld()->GetTimeSeconds();

    if (CurrentTime < NextFireTime)
    {
        return false;
    }

    return true;
}