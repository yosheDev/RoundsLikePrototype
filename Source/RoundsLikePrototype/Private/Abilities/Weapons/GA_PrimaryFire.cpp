// Copyrighted Jacob Jones 2026

// Preprocessor directives
#include "Abilities/Weapons/GA_PrimaryFire.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "FPSCharacter.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"
#include "Weapons/FireData.h"
#include "Weapons/AmmoComponent.h"

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
    
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("FireLog: [%s] ACTIVATE PrimaryFire | AbilityActive=%s"),
        ActorInfo->AvatarActor.Get()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"),
        IsActive() ? TEXT("TRUE") : TEXT("FALSE")
    );

    // Is there an avatar actor?
    if (AActor* Avatar = ActorInfo->AvatarActor.Get())
    {
        #pragma region Weapon and Ammo Validation

        if (!Avatar->Implements<UWeaponHolder>())
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        AProjectileWeapon* Weapon = IWeaponHolder::Execute_GetEquippedWeapon(Avatar);
        if (Weapon == nullptr || !(Weapon->CanFire()))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        UAmmoComponent* AmmoComponent = Weapon->GetAmmoComponent();
        if (!AmmoComponent)
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        if (!AmmoComponent->HasAmmo())
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        FString RoleString = ActorInfo->AvatarActor.Get()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
        UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: Has passed weapon check"), *RoleString);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%s Ammo: %d"), *RoleString, AmmoComponent->CurrentAmmo));
        #pragma endregion

        Weapon->OnBurstComplete.AddUObject(this, &UGA_PrimaryFire::OnBurstComplete);
        bStopAfterBurst = false;
        bBurstComplete = false;

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

        const EWeaponFireType FireType = static_cast<EWeaponFireType>(Attributes->GetWeaponFireType());
        if (FireType != EWeaponFireType::Burst)
        {
            ScheduleNextShot();
        }

        RoleString = Avatar->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
        UE_LOG(LogTemp, Log, TEXT("FireLog: [%s]: End of Primary Fire Ability. Weapon is [%s]"), *RoleString, Weapon ? *Weapon->GetName() : TEXT("NULL"));
    }
}

void UGA_PrimaryFire::EndAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    bool bReplicateEndAbility, 
    bool bWasCancelled)
{
    #pragma region Unbind Delegates
    if (CurrentActorInfo)
    {
        if (AActor* Avatar = CurrentActorInfo->AvatarActor.Get())
        {
            if (Avatar->Implements<UWeaponHolder>())
            {
                if (AProjectileWeapon* Weapon = IWeaponHolder::Execute_GetEquippedWeapon(Avatar))
                {
                    Weapon->OnBurstComplete.RemoveAll(this);
                }
            }
        }
    }
    #pragma endregion

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(FireTimerHandle);
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("FireLog: [%s] END PrimaryFire | Cancelled=%s"),
        ActorInfo && ActorInfo->AvatarActor.IsValid() &&
        ActorInfo->AvatarActor.Get()->HasAuthority()
        ? TEXT("SERVER")
        : TEXT("CLIENT"),
        bWasCancelled ? TEXT("TRUE") : TEXT("FALSE")
    );

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_PrimaryFire::ScheduleNextShot()
{
    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    if (!(CurrentActorInfo->AvatarActor.Get()->Implements<UWeaponHolder>()))
    {
        return;
    }

    AProjectileWeapon* Weapon = IWeaponHolder::Execute_GetEquippedWeapon(CurrentActorInfo->AvatarActor.Get());
    if (Weapon == nullptr || !(Weapon->CanFire()))
    {
        return;
    }

    UAmmoComponent* AmmoComponent = Weapon->GetAmmoComponent();
    if (!AmmoComponent)
    {
        return;
    }
    if (!AmmoComponent->HasAmmo())
    {
        EndAbility(
            CurrentSpecHandle,
            CurrentActorInfo,
            CurrentActivationInfo,
            true,
            false
        );

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

    AActor* Avatar = CurrentActorInfo->AvatarActor.Get();
    if (!IsValid(Avatar))
    {
        return;
    }

    AProjectileWeapon* Weapon = IWeaponHolder::Execute_GetEquippedWeapon(Avatar);
    UAmmoComponent* AmmoComponent = Weapon->GetAmmoComponent();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("FireLog: [%s] FireShot() | CurrentAmmo=%d | ClientPredictedAmmo=%d"),
        CurrentActorInfo->AvatarActor.Get()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"),
        AmmoComponent ? AmmoComponent->CurrentAmmo : -1,
        AmmoComponent ? AmmoComponent->ClientPredictedAmmo : -1);

    // NOTE need to move this to only fire when actually firing a shot from the weapon.
    // Do not fire if there is not enough ammo.
    if (!AmmoComponent->HasAmmo())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("FireLog: [%s] NO AMMO -> ENDING PRIMARY FIRE"),
            Avatar->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT")
        );

        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    LastFireTime = GetWorld()->GetTimeSeconds();
    NextFireTime = LastFireTime + GetFireInterval();

    bBurstComplete = false;

    #pragma region Activate Primary Fire
    APlayerController* PC = CurrentActorInfo->PlayerController.Get();
    FVector AimLocation = PC->PlayerCameraManager->GetCameraLocation();
    FRotator AimRotation = PC->PlayerCameraManager->GetCameraRotation().Vector().Rotation();
    AimRotation.Pitch += Attributes->GetBulletArc();
    SpawnTransform = FTransform(AimRotation, AimLocation);

    // Create SpawnTransform of SpawnData here. Weapon unique properties(spread, stats, bullets) will propograte in the AProjectileWeapon.
    FProjectileSpawnData SpawnData;
    SpawnData.SpawnTransform = SpawnTransform;

    // Create FireData based on attributes and conditions.
    FFireData FireData = MakeFireData();

    Weapon->PrimaryFire(CurrentSpecHandle, CurrentActivationInfo, SpawnData, FireData);

    #pragma region TryActivate BulletJump
    // TryActivate BulletJump Ability. Ability will handle whether or not activates based on grounded state.

    AFPSCharacter* Character = Cast<AFPSCharacter>(Avatar);
    if (Character)
    {
        UFPSAbilitySystemComponent* FPSAbilitySystemComponent = Cast<UFPSAbilitySystemComponent>(Character->GetAbilitySystemComponent());
        if (FPSAbilitySystemComponent)
        {
            FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Movement.BulletJump"));
            FGameplayTagContainer TagContainer;
            TagContainer.AddTag(AbilityTag);

            FPSAbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
        }
    }
    #pragma endregion
    #pragma endregion

    #pragma region Schedule Next Shot (if not a burst weapon)
    const EWeaponFireType FireType = static_cast<EWeaponFireType>(Attributes->GetWeaponFireType());

    if (FireType != EWeaponFireType::Burst)
    {
        ScheduleNextShot();
    }
    #pragma endregion
}

void UGA_PrimaryFire::StopFiring()
{
    const EWeaponFireType FireType = static_cast<EWeaponFireType>(Attributes->GetWeaponFireType());

    if (FireType == EWeaponFireType::Burst)
    {
        if (bBurstComplete)
        {
            // The current burst has already finished,
            // so releasing fire means we're done.
            EndAbility(
                CurrentSpecHandle,
                CurrentActorInfo,
                CurrentActivationInfo,
                true,
                false
            );
        }
        else
        {
            // Current burst is still running.
            // Let it finish, then OnBurstComplete() will end us.
            bStopAfterBurst = true;
        }
        return;
    }

    EndAbility(
        CurrentSpecHandle,
        CurrentActorInfo,
        CurrentActivationInfo,
        true,
        false
    );
}

void UGA_PrimaryFire::OnBurstComplete()
{
    UE_LOG(LogTemp, Warning, TEXT("FireLog: OnBurstComplete | StopAfterBurst=%s"), bStopAfterBurst ? TEXT("TRUE") : TEXT("FALSE"));
    if (bStopAfterBurst)
    {
        EndAbility(
            CurrentSpecHandle,
            CurrentActorInfo,
            CurrentActivationInfo,
            true,
            false
        );

        return;
    }

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

FFireData UGA_PrimaryFire::MakeFireData()
{
    if (AActor* Avatar = CurrentActorInfo->AvatarActor.Get())
    {
        if (!Avatar->Implements<UWeaponHolder>())
        {
            return FFireData();
        }

        AProjectileWeapon* Weapon = IWeaponHolder::Execute_GetEquippedWeapon(Avatar);
        if (Weapon == nullptr)
        {
            return FFireData();
        }

        UAmmoComponent* AmmoComponent = Weapon->GetAmmoComponent();
        if (!AmmoComponent)
        {
            return FFireData();
        }

        Attributes = GetAbilitySystemComponentFromActorInfo()->GetSet<UGunplayAttributeSet>();

        FFireData FireData;
        FireData.WeaponFireType = static_cast<EWeaponFireType>(Attributes->GetWeaponFireType());
        FireData.FireBurstAmount = FMath::Min(AmmoComponent->GetCurrentAmmo(), Attributes->GetFireBurstAmount());
        FireData.FireBurstInterval = Attributes->GetBurstFireRate();
        FireData.FireBulletAmount = FMath::Min(AmmoComponent->GetCurrentAmmo(), Attributes->GetFireShotAmount());
        FireData.FireSpreadXAngle = Attributes->GetFireSpreadXAngle();
        FireData.FireSpreadZAngle = Attributes->GetFireSpreadZAngle();

        return FireData;
    }

    return FFireData();
}