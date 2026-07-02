// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Preprocessor Directives
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

// Forward Declarations
class UInputMappingContext;
class AFPSCharacter;
class UShooterBulletCounterUI;

/**
 *  Simple PlayerController for a first person shooter game
 *  Manages input mappings
 *  Respawns the player pawn when it's destroyed
 */
UCLASS(abstract, config = "Game")
class ROUNDSLIKEPROTOTYPE_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Input mapping contexts for this player */
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Character class to respawn when the possessed pawn is destroyed */
	UPROPERTY(EditAnywhere, Category = "Shooter|Respawn")
	TSubclassOf<AFPSCharacter> CharacterClass;

	/** Tag to grant the possessed pawn to flag it as the player */
	UPROPERTY(EditAnywhere, Category = "Shooter|Player")
	FName PlayerPawnTag = FName("Player");

protected:

	/** Gameplay Initialization */
	virtual void BeginPlay() override;

	/** Initialize input bindings */
	virtual void SetupInputComponent() override;

	/** Pawn initialization */
	virtual void OnPossess(APawn* InPawn) override;

	/** Called if the possessed pawn is destroyed */
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);

	/** Called when the possessed pawn is damaged */
	UFUNCTION()
	void OnPawnDamaged(float LifePercent);
};
