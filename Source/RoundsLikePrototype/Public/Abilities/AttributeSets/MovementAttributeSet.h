// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "MovementAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API UMovementAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// Max movement speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData MaxSpeed;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, MaxSpeed);

	// JumpZVelocity
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData JumpStrength;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, JumpStrength);
	
	// Maximum number of jumps allowed before touching ground again.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData JumpCount;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, JumpCount);

	// Scale of gravity.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData GravityScale;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, GravityScale);

	// Half height when crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData CrouchedHalfHeight;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, CrouchedHalfHeight);

//WallRunningInit : Initiation of wallrunning. (Workshop the name ? )
//DashingInit : Initiation of dashing. (Workshop the name ? )
//BulletJumpInit : Initiation of bullet jumping.

};
