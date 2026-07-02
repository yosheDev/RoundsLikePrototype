// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Preprocessor Directives
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "FPSPlayerController.generated.h"

// Forward Declarations
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AFPSCharacter;


UCLASS(abstract, config = "Game")
class ROUNDSLIKEPROTOTYPE_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	#pragma region Input System
	/** Default input mapping contexts for players */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultInputMappingContexts;

	#pragma region Input Actions
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* PrimaryFireAction;
	#pragma endregion

	/** Set up input action bindings */
	virtual void SetupInputComponent() override;

	virtual void SetupMappingContexts();

	#pragma endregion

	/** Character class to respawn when the possessed pawn is destroyed */
	UPROPERTY(EditAnywhere, Category = "Shooter|Respawn")
	TSubclassOf<AFPSCharacter> CharacterClass;

	/** Tag to grant the possessed pawn to flag it as the player */
	UPROPERTY(EditAnywhere, Category = "Shooter|Player")
	FName PlayerPawnTag = FName("Player");
	
	

protected:
	#pragma region Input Functions
	/** Called from Input Actions for movement input */
	virtual void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	virtual void LookInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	virtual void JumpInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	virtual void PrimaryFireInput(const FInputActionValue& Value);
	#pragma endregion

	/** Gameplay Initialization */
	virtual void BeginPlay() override;

	/** Pawn initialization */
	virtual void OnPossess(APawn* InPawn) override;

	/** Called if the possessed pawn is destroyed */
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);

	/** Called when the possessed pawn is damaged */
	UFUNCTION()
	void OnPawnDamaged(float LifePercent);
};
