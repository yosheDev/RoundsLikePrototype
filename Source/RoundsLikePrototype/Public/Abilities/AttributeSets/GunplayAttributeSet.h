
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
	// Maximum amount of damage each bullet can deal.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData BulletImpactDamage;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletImpactDamage);

	// Influence factor for bullet jumping.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletJumpFactor;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletJumpFactor);

	// Velocity that bullets travel at.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSpeed;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletSpeed);

	// Size of the bullet.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSize;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletSize);

	// Arc for the bullet to follow.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletGravity;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletGravity);

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
	// Amount of bullets fired for each shot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletAmount;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletAmount);

	// Type for the bullet spread(shotgun, fan, single, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSpreadType;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletSpreadType);

	// Regeneration rate of individual bullets for clip.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletRegen;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletRegen);

	// Fire rate for auto fire.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData AutoFireRate;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, AutoFireRate);

	// Maximum amount of bullets in each clip.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData ClipCapacity;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, ClipCapacity);

	// Recoil factor per shot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData RecoilFactor;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, RecoilFactor);
	#pragma endregion
};
