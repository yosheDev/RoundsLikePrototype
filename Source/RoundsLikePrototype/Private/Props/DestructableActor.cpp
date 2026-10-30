// Copyright Jacob Jones 2026


#include "Props/DestructableActor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Components/HealthComponent.h"
#include "Abilities/AttributeSets/VitalityAttributeSet.h"

ADestructableActor::ADestructableActor()
{
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    AbilitySystemComponent = CreateDefaultSubobject<UFPSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    VitalityAttributeSet = CreateDefaultSubobject<UVitalityAttributeSet>(TEXT("VitalityAttributeSet"));

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ADestructableActor::BeginPlay()
{
	Super::BeginPlay();
	
    // Initialize health attributes.
    AbilitySystemComponent->SetNumericAttributeBase(UVitalityAttributeSet::GetMaxHealthAttribute(), MaxHealth);
    AbilitySystemComponent->SetNumericAttributeBase(UVitalityAttributeSet::GetHealthAttribute(), MaxHealth);

    HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);

    HealthComponent->OnDamage.AddDynamic(this, &ADestructableActor::OnDamage);
    HealthComponent->OnDeath.AddDynamic(this, &ADestructableActor::OnDestroy);
}

void ADestructableActor::OnDamage_Implementation(float DamageAmount, float NewHealth)
{

}

void ADestructableActor::OnDestroy_Implementation()
{

}

UAbilitySystemComponent* ADestructableActor::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

