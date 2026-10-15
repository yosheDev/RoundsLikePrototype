// Copyright Jacob Jones 2026


#include "Abilities/Bullet/PeriodicVolumeDamage.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameplayEffect.h"

APeriodicVolumeDamage::APeriodicVolumeDamage()
{
    PrimaryActorTick.bCanEverTick = false;

    DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageSphere"));
    SetRootComponent(DamageSphere);

    DamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    DamageSphere->SetGenerateOverlapEvents(true);

    DamageSphere->OnComponentBeginOverlap.AddDynamic(this, &APeriodicVolumeDamage::OnVolumeBeginOverlap);
    DamageSphere->OnComponentEndOverlap.AddDynamic(this, &APeriodicVolumeDamage::OnVolumeEndOverlap);

    bReplicates = true;
}

void APeriodicVolumeDamage::BeginPlay()
{
    Super::BeginPlay();

    if (VolumeDuration > 0.0f)
    {
        SetLifeSpan(VolumeDuration);
    }
}

void APeriodicVolumeDamage::OnVolumeBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // This is the server and never ever overlap self.
    if (!HasAuthority() || !IsValid(OtherActor) || OtherActor == this)
    {
        return;
    }

    if (UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
    {
        OverlappingActors.Add(OtherActor);
        StartDamageTimerIfNeeded();
    }
}

void APeriodicVolumeDamage::OnVolumeEndOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex)
{
    // Must be server
    if (!HasAuthority())
    {
        return;
    }

    OverlappingActors.Remove(OtherActor);
    StopDamageTimerIfEmpty();
}

void APeriodicVolumeDamage::StartDamageTimerIfNeeded()
{
    if (!GetWorld()->GetTimerManager().IsTimerActive(DamageTimerHandle))
    {
        GetWorld()->GetTimerManager().SetTimer(
            DamageTimerHandle,
            this,
            &APeriodicVolumeDamage::ApplyPeriodicDamage,
            DamageInterval,
            true);
    }
}

void APeriodicVolumeDamage::StopDamageTimerIfEmpty()
{
    for (auto It = OverlappingActors.CreateIterator(); It; ++It)
    {
        if (!IsValid(*It))
        {
            It.RemoveCurrent();
        }
    }

    if (OverlappingActors.IsEmpty())
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
    }
}

void APeriodicVolumeDamage::ApplyPeriodicDamage()
{
    if (!HasAuthority() || !DamageEffectClass)
    {
        return;
    }

    for (auto It = OverlappingActors.CreateIterator(); It; ++It)
    {
        AActor* Target = It->Get();

        if (!IsValid(Target))
        {
            It.RemoveCurrent();
            continue;
        }

        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);

        if (!TargetASC)
        {
            It.RemoveCurrent();
            continue;
        }

        // Resolve the source ASC from the Volume's owner.
        AActor* SourceActor = GetOwner();
        UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);

        if (!SourceASC)
        {
            SourceASC = TargetASC;
        }

        FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();

        Context.AddSourceObject(this);

        FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffectClass, DamageEffectLevel, Context);

        if (Spec.IsValid())
        {
            SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
        }
    }
}

