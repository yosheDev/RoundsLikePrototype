// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GunplayAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API UGunplayAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// Maximum amount of damage each bullet can deal.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayAttributeData BulletDamage;

	// Influence factor for bullet jumping.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletJumpFactor;

	// Velocity that bullets travel at.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSpeed;

	// Size of the bullet.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSize;

	// Arc for the bullet to follow.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletArc;

	// Chance for a critical strike upon impact.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletCritChance;

	// Amount to lifesteal.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletLifestealAmount;

	// The type of the bullet(bouncy, seeking, explosive, etc.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletType;

	// Amount of bullets fired for each shot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletAmount;

	// Type for the bullet spread(shotgun, fan, single, etc.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletSpreadType;

	// Regeneration rate of individual bullets for clip.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData BulletRegen;

	// Maximum amount of bullets in each clip.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData ClipCapacity;
	
	// Recoil factor per shot.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData RecoilFactor;

};
