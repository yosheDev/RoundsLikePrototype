
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "GunplayAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API UGunplayAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	UGunplayAttributeSet();

	#pragma region Bullet Attributes
	#pragma region Bullet Damage
	// Maximum amount of damage each bullet can deal.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletImpactDamage;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletImpactDamage);

	// Distance at which far distance falloff begins.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletDamageFalloffStartDistance;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletDamageFalloffStartDistance);

	// End of distance falloff range.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletDamageFalloffEndDistance;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletDamageFalloffEndDistance);

	// Modifier at max distance.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletDamageFarModifier;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletDamageFarModifier);

	// Distance at which near distance falloff begins.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletDamageReverseFalloffStartDistance;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletDamageReverseFalloffStartDistance);

	// End of distance reverse falloff range (for close-range modifications.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletDamageReverseFalloffEndDistance;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletDamageReverseFalloffEndDistance);

	// Modifier at min distance.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletDamageNearModifier;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletDamageNearModifier);

	#pragma endregion

	// Influence factor for bullet jumping.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletJumpFactor;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletJumpFactor);

	// Size of the bullet.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSize;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletSize);

	// Velocity that bullets travel at.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSpeed;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletSpeed);

	// Max arc height distance from firing forward.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletArc;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletArc);

	// How far bullet travels to return path to match firing forward.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletArcDistance;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletArcDistance);

	// How much influence the designed arc will have on bullet trajectory.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletArcPitchInfluence;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletArcPitchInfluence);

	// Arc for the bullet to follow.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletGravity;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletGravity);

	// Number of bounces projectile can do.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletBounceAmount;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletBounceAmount);

	// Multiplied with velocity to determine retention of it.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletBounceVelocityRetention;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletBounceVelocityRetention);

	// Chance for a critical strike upon impact.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletCritChance;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletCritChance);

	// Amount to lifesteal.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletLifestealAmount;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletLifestealAmount);

	// The type of the bullet(bouncy, seeking, explosive, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletType;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletType);

#pragma endregion

	#pragma region Fire Attributes

	// Type for the bullet spread(0 = Standard, 1 = Burst, 2 = Automatic, 3 = Shotgun Spread, 4 = Charge Cannon?)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData WeaponFireType;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, WeaponFireType);

	// Maximum amount of bullets in each clip.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData ClipCapacity;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, ClipCapacity);

	// Recoil factor per shot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData RecoilFactor;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, RecoilFactor);

	// Amount of bullets fired for each shot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData FireShotAmount;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, FireShotAmount);

	// Amount of time projectiles are spawned for each fire (for burst shots).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData FireBurstAmount;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, FireBurstAmount);

	// X Angle variance for fire.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData FireSpreadXAngle;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, FireSpreadXAngle);

	// Z Angle variance for fire.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData FireSpreadZAngle;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, FireSpreadZAngle);

	// Regeneration rate of individual bullets for clip.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletRegen;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletRegen);

	// Fire rate for burst fire.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BurstFireRate;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BurstFireRate);

	// Fire rate for auto fire.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData AutoFireRate;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, AutoFireRate);
	#pragma endregion
};
