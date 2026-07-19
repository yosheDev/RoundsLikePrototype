
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

	// Maximum amount of damage each bullet can deal.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData BulletImpactDamage;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletImpactDamage);

	// Influence factor for bullet jumping.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletJumpFactor;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletJumpFactor);

	// Velocity that bullets travel at.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSpeed;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletSpeed);

	// Size of the bullet.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSize;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletSize);

	// Arc for the bullet to follow.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletGravity;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletGravity);

	// Chance for a critical strike upon impact.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletCritChance;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletCritChance);

	// Amount to lifesteal.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletLifestealAmount;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletLifestealAmount);

	// The type of the bullet(bouncy, seeking, explosive, etc.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletType;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletType);

	// Amount of bullets fired for each shot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletAmount;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletAmount);

	// Type for the bullet spread(shotgun, fan, single, etc.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSpreadType;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletSpreadType);

	// Regeneration rate of individual bullets for clip.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletRegen;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, BulletRegen);

	// Maximum amount of bullets in each clip.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData ClipCapacity;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, ClipCapacity);

	// Recoil factor per shot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData RecoilFactor;
	ATTRIBUTE_ACCESSORS(UGunplayAttributeSet, RecoilFactor);
};
