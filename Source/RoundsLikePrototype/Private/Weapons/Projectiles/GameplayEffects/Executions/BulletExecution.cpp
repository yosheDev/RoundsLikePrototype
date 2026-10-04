// Copyright Jacob Jones 2026


#include "Weapons/Projectiles/GameplayEffects/Executions/BulletExecution.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "Abilities/AttributeSets/VitalityAttributeSet.h"

struct FDamageStatics
{
    // Declare capture definitions.
    DECLARE_ATTRIBUTE_CAPTUREDEF(BulletImpactDamage);

    DECLARE_ATTRIBUTE_CAPTUREDEF(BulletDamageFalloffStartDistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BulletDamageFalloffEndDistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BulletDamageFarModifier);

    DECLARE_ATTRIBUTE_CAPTUREDEF(BulletDamageReverseFalloffStartDistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BulletDamageReverseFalloffEndDistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(BulletDamageNearModifier);

    FDamageStatics()
    {
        // Define capture definitions.
        // DEFINE_ATTRIBUTE_CAPTUREDEF(AttributeSet Source Class, AttributeName, Source/Target(shooter or the victim), bSnapshot(If false, uses value at time of calc. If true, takes snapshot whenver spec is created.))
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGunplayAttributeSet, BulletImpactDamage, Source, true);

        DEFINE_ATTRIBUTE_CAPTUREDEF(UGunplayAttributeSet, BulletDamageFalloffStartDistance, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGunplayAttributeSet, BulletDamageFalloffEndDistance, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGunplayAttributeSet, BulletDamageFarModifier, Source, true);

        DEFINE_ATTRIBUTE_CAPTUREDEF(UGunplayAttributeSet, BulletDamageReverseFalloffStartDistance, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGunplayAttributeSet, BulletDamageReverseFalloffEndDistance, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGunplayAttributeSet, BulletDamageNearModifier, Source, true);
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

    RelevantAttributesToCapture.Add(DamageStatics().BulletDamageFalloffStartDistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().BulletDamageFalloffEndDistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().BulletDamageFarModifierDef);

    RelevantAttributesToCapture.Add(DamageStatics().BulletDamageReverseFalloffStartDistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().BulletDamageReverseFalloffEndDistanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().BulletDamageNearModifierDef);
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

    #pragma region Get Attribute Values
    float BulletImpactDamage = 1.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BulletImpactDamageDef, Params, BulletImpactDamage);

    // Reverse falloff — close range
    float ReverseFalloffStartDistance = 0.0f;
    float ReverseFalloffEndDistance = 0.0f;
    float NearModifier = 1.0f;

    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BulletDamageReverseFalloffStartDistanceDef, Params, ReverseFalloffStartDistance);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BulletDamageReverseFalloffEndDistanceDef, Params, ReverseFalloffEndDistance);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BulletDamageNearModifierDef, Params, NearModifier);

    // Normal falloff — long range
    float FalloffStartDistance = 0.0f;
    float FalloffEndDistance = 0.0f;
    float FarModifier = 1.0f;

    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BulletDamageFalloffStartDistanceDef, Params, FalloffStartDistance);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BulletDamageFalloffEndDistanceDef, Params, FalloffEndDistance);
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BulletDamageFarModifierDef, Params, FarModifier);
    #pragma endregion

    #pragma region Get Custom Spec Values
    const FGameplayTag TravelDistanceTag = FGameplayTag::RequestGameplayTag(TEXT("Data.BulletTravelDistance"));
    const float BulletTravelDistance = Spec.GetSetByCallerMagnitude(TravelDistanceTag, false, 0.0f);
    #pragma endregion

    #pragma region Calculate Damage Falloff
    float RangeDamageModifier = 1.0f;
    /*UE_LOG(LogTemp, Warning, TEXT("FireLog: BulletTravelDistance = %f"), BulletTravelDistance);
    UE_LOG(LogTemp, Log, TEXT("FireLog: ReverseFalloffStartDistance: [%f]"), ReverseFalloffStartDistance);
    UE_LOG(LogTemp, Warning, TEXT("FireLog: ReverseFalloffEndDistance = %f"), ReverseFalloffEndDistance);
    UE_LOG(LogTemp, Warning, TEXT("FireLog: FalloffStartDistance = %f"), FalloffStartDistance);
    UE_LOG(LogTemp, Warning, TEXT("FireLog: FalloffEndDistance = %f"), FalloffEndDistance);
    UE_LOG(LogTemp, Warning, TEXT("FireLog: Reverse condition = %s"), BulletTravelDistance < ReverseFalloffEndDistance ? TEXT("TRUE") : TEXT("FALSE"));
    UE_LOG(LogTemp, Warning, TEXT("FireLog: Far condition = %s"), BulletTravelDistance > FalloffStartDistance ? TEXT("TRUE") : TEXT("FALSE"));
    UE_LOG(LogTemp, Warning, TEXT("FireLog: FalloffRange = %f"), FalloffEndDistance - FalloffStartDistance);*/

    // Reverse / close-range falloff
    if (BulletTravelDistance < ReverseFalloffEndDistance)
    {
        const float ReverseFalloffRange = ReverseFalloffEndDistance - ReverseFalloffStartDistance;

        if (ReverseFalloffRange > KINDA_SMALL_NUMBER)
        {
            const float Alpha = FMath::Clamp((BulletTravelDistance - ReverseFalloffStartDistance) / ReverseFalloffRange, 0.0f, 1.0f);
            RangeDamageModifier = FMath::Lerp(NearModifier, 1.0f, Alpha);
            //UE_LOG(LogTemp, Log, TEXT("FireLog: Near Range Damage Modifier: [%f]"), RangeDamageModifier);
        }
    }
    // Normal / far-range falloff
    else if (BulletTravelDistance > FalloffStartDistance)
    {
        const float FalloffRange = FalloffEndDistance - FalloffStartDistance;

        if (FalloffRange > KINDA_SMALL_NUMBER)
        {
            const float Alpha = FMath::Clamp((BulletTravelDistance - FalloffStartDistance) / FalloffRange, 0.0f, 1.0f);
            RangeDamageModifier = FMath::Lerp(1.0f, FarModifier, Alpha);
            //UE_LOG(LogTemp, Log, TEXT("FireLog: Far Range Damage Modifier: [%f]"), RangeDamageModifier);
        }
    }
    #pragma endregion

    float FinalDamage = BulletImpactDamage * RangeDamageModifier;


    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVitalityAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
}