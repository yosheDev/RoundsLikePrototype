// Copyright Jacob Jones 2026


#include "Components/HealthComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/AttributeSets/VitalityAttributeSet.h"


UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}


void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    // Automatically retrieve the ASC from the owning actor.
    if (AActor* Owner = GetOwner())
    {
        if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Owner))
        {
            InitializeWithAbilitySystem(AbilitySystemInterface->GetAbilitySystemComponent());
        }
    }
}


void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindFromAbilitySystem();

    Super::EndPlay(EndPlayReason);
}


void UHealthComponent::InitializeWithAbilitySystem(UAbilitySystemComponent* InASC)
{
    if (!InASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("HealthComponent: InitializeWithAbilitySystem received a null ASC."));

        return;
    }

    // Avoid duplicate bindings if already initialized with this ASC.
    if (AbilitySystemComponent == InASC && VitalityAttributeSet && HealthChangedDelegateHandle.IsValid())
    {
        return;
    }

    // Remove any existing binding.
    UnbindFromAbilitySystem();

    AbilitySystemComponent = InASC;

    // Retrieve the Vitality AttributeSet.
    VitalityAttributeSet = AbilitySystemComponent->GetSet<UVitalityAttributeSet>();

    if (!VitalityAttributeSet)
    {
        UE_LOG(LogTemp, Warning, TEXT("HealthComponent: UVitalityAttributeSet not found on ASC."));

        AbilitySystemComponent = nullptr;
        return;
    }

    // Bind to health attribute changes.
    HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        UVitalityAttributeSet::GetHealthAttribute()).AddUObject(this, &UHealthComponent::HandleHealthChanged);

    // Initialize death state from current health.
    bIsDead = GetHealth() <= 0.0f;

    UE_LOG(LogTemp, Log,
        TEXT("HealthComponent: Initialized. Health: %.2f / %.2f"),
        GetHealth(),
        GetMaxHealth()
    );
}

void UHealthComponent::Heal(float HealAmount)
{
    if (!AbilitySystemComponent || !VitalityAttributeSet)
    {
        return;
    }

    if (HealAmount <= 0.0f)
    {
        return;
    }

    AbilitySystemComponent->ApplyModToAttribute(
        UVitalityAttributeSet::GetHealthAttribute(),
        EGameplayModOp::Additive,
        HealAmount
    );

    if (bIsDead && GetHealth() > 0.0f)
    {
        bIsDead = false;
    }
}

void UHealthComponent::UnbindFromAbilitySystem()
{
    if (AbilitySystemComponent && HealthChangedDelegateHandle.IsValid())
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UVitalityAttributeSet::GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
    }

    HealthChangedDelegateHandle.Reset();

    VitalityAttributeSet = nullptr;
    AbilitySystemComponent = nullptr;
}


void UHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
    const float OldHealth = Data.OldValue;
    const float NewHealth = Data.NewValue;

    // Health decreased.
    if (NewHealth < OldHealth)
    {
        const float DamageAmount = OldHealth - NewHealth;

        OnDamage.Broadcast(DamageAmount, NewHealth);
    }

    // Health reached zero.
    if (NewHealth <= 0.0f)
    {
        if (!bIsDead)
        {
            bIsDead = true;

            OnDeath.Broadcast();
        }
    }
    else
    {
        // Allow the actor to be revived if health is restored.
        bIsDead = false;
    }
}


float UHealthComponent::GetHealth() const
{
    if (!VitalityAttributeSet)
    {
        return 0.0f;
    }

    return VitalityAttributeSet->GetHealth();
}


float UHealthComponent::GetMaxHealth() const
{
    if (!VitalityAttributeSet)
    {
        return 0.0f;
    }

    return VitalityAttributeSet->GetMaxHealth();
}


bool UHealthComponent::IsDead() const
{
    return bIsDead;
}
