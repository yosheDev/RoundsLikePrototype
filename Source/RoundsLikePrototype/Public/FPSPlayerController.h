// Copyrighted Jacob Jones 2026

#pragma once

// Preprocessor Directives
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSHudController.h"
#include "EnhancedInputSubsystems.h"
#include "FPSGameState.h"
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
public:
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	AFPSCharacter* PlayerCharacter;

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

	/** Tag to grant the possessed pawn to flag it as the player */
	UPROPERTY(EditAnywhere, Category = "Player")
	FName PlayerPawnTag = FName("Player");
	
	
public:
	UFUNCTION(Client, Reliable)
	void Client_ShowDraftScreen();

	UFUNCTION(Client, Reliable)
	void Client_SetCanSelectUI(bool CanSelectUI);

	UFUNCTION(Server, Reliable)
	void Server_NotifyRoundEndComplete();

	UFUNCTION(Server, Reliable)
	void Server_FinishedDraft();

	UFUNCTION()
	void OnMatchPhaseChanged(EMatchPhase NewPhase);

protected:
	#pragma region Input Functions
	virtual void MoveInput(const FInputActionValue& Value);
	virtual void LookInput(const FInputActionValue& Value);

	virtual void JumpInputStarted(const FInputActionValue& Value);
	virtual void JumpInputCompleted(const FInputActionValue& Value);

	virtual void PrimaryFireInputStarted(const FInputActionValue& Value);
	virtual void PrimaryFireInputTriggered(const FInputActionValue& Value);
	#pragma endregion

	/** Gameplay Initialization */
	virtual void BeginPlay() override;

	/** Pawn Initialization */
	virtual void OnPossess(APawn* InPawn) override;

	/** Called if the possessed pawn is destroyed */
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);
};
