// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "MovementAttributeSet.generated.h"

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData JumpStrength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData JumpCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData GravityScale;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGameplayAttributeData CrouchedHalfHeight;

//MaxSpeed: Max walk speed.
//JumpStrength : JumpZVelocity.
//JumpCount : Maximum number of jumps allowed before touching ground again.
//GravityScale : Scale of gravity.
//CrouchHalfHeight : Half height when crouching
//WallRunningInit : Initiation of wallrunning. (Workshop the name ? )
//DashingInit : Initiation of dashing. (Workshop the name ? )
//BulletJumpInit : Initiation of bullet jumping.

};
