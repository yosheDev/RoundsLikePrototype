// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// Forward Declarations
class UAbilitySystemComponent;
class UVitalityAttributeSet;
struct FOnAttributeChangeData;

// Health delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamage, float, DamageAmount, float, NewHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);


/**
 * Component responsible for monitoring GAS health
 * and broadcasting health-related events.
 *
 * The owning actor must implement IAbilitySystemInterface
 * and provide an ASC containing UVitalityAttributeSet.
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class ROUNDSLIKEPROTOTYPE_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UHealthComponent();

    void InitializeWithAbilitySystem(UAbilitySystemComponent* InASC);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float HealAmount);

#pragma region Delegates
public:

    // Called whenever health decreases.
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDamage OnDamage;

    // Called when health reaches zero.
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDeath OnDeath;

private:

    // Delegate handle for the health attribute OnHealthChanged. Used for event that calls delegates OnDamage and OnDeath.
    FDelegateHandle HealthChangedDelegateHandle;

#pragma endregion

#pragma region Utility
public:
    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealth() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsDead() const;
#pragma endregion

protected:

    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    const UVitalityAttributeSet* VitalityAttributeSet = nullptr;

    bool bIsDead = false;

    // GAS health change callback.
    void HandleHealthChanged(const FOnAttributeChangeData& Data);

    void UnbindFromAbilitySystem();
};
