// Copyright Jacob Jones 2026


#include "Weapons/Projectiles/GameplayEffects/Executions/BulletExecution.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "Abilities/AttributeSets/VitalityAttributeSet.h"

struct FDamageStatics
{
    // Declare capture definitions.
    DECLARE_ATTRIBUTE_CAPTUREDEF(BulletImpactDamage);

    FDamageStatics()
    {
        // Define capture definitions.
        // DEFINE_ATTRIBUTE_CAPTUREDEF(AttributeSet Source Class, AttributeName, Source/Target(shooter or the victim), bSnapshot(If false, uses value at time of calc. If true, takes snapshot whenver spec is created.))
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGunplayAttributeSet, BulletImpactDamage, Source, true);
    }
};

static const FDamageStatics& DamageStatics()
{
    static FDamageStatics DStatics;
    return DStatics;
}

UBulletExecution::UBulletExecution()
{
    // Register which attributes are needed when this calculation runs.
    RelevantAttributesToCapture.Add(DamageStatics().BulletImpactDamageDef);
}

// EffectContext:
// SourceObject is the weapon
// Instigator is the shooters pawn.

void UBulletExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    //@param ExecutionParams     Contains incoming data including Source / Target tags, specs, and captured attributes.
    //@param OutExecutionOutput  Output container where you push final calculated modifiers back to the GAS framework.

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FAggregatorEvaluateParameters Params;

    // Retrive BulletImpactDamage
    float BulletImpactDamage = 1.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BulletImpactDamageDef, Params, BulletImpactDamage);

    float FinalDamage = BulletImpactDamage;


    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVitalityAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
}