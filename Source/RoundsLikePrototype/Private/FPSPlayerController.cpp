// Copyrighted Jacob Jones 2026


#include "FPSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "FPSHudController.h"
#include "FPSCharacter.h"
#include "EnhancedInputComponent.h"
#include "FPSGameState.h"
#include "FPSGameMode.h"
#include "FPSPlayerState.h"
#include "RoundsLikePrototype.h"

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<AFPSCharacter>(GetPawn());
	SetupMappingContexts(); // Set up Mapping Contexts
}

void AFPSPlayerController::SetupMappingContexts()
{
	// Only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			// Add all default contexts to the local player enhanced input subsystem.
			for (UInputMappingContext* CurrentContext : DefaultInputMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void AFPSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// subscribe to the pawn's OnDestroyed delegate
	//InPawn->OnDestroyed.AddDynamic(this, &AFPSPlayerController::OnPawnDestroyed);

	// is this a shooter character?
	if (AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(InPawn))
	{
		// add the player tag
		FPSCharacter->Tags.Add(PlayerPawnTag);

		// subscribe to the pawn's delegates
		//FPSCharacter->OnBulletCountUpdated.AddDynamic(this, &AFPSPlayerController::OnBulletCountUpdated);
		//FPSCharacter->OnDamaged.AddDynamic(this, &AFPSPlayerController::OnPawnDamaged);

		// force update the life bar
		//FPSCharacter->OnDamaged.Broadcast(1.0f);
	}
}

void AFPSPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	
}

void AFPSPlayerController::OnMatchPhaseChanged(EMatchPhase NewPhase)
{
	UE_LOG(LogTemp, Log, TEXT("[%s]: OnMatchPhaseChanged() to [%s]"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), *UEnum::GetValueAsString(NewPhase));
	switch (NewPhase)
	{
		case EMatchPhase::RoundStarting:
		{
			// Disable Gameplay Input
			DisableInput(this);
			break;
		}
		case EMatchPhase::InRound:
		{
			// Enable Gameplay Input
			EnableInput(this);
			break;
		}
		case EMatchPhase::RoundEnd:
		{
			/* Perform and finish any animations, sequences, UI, etc. before calling ServerNotifyRoundEndComplete(). */

			AFPSGameMode* FPSGameMode = Cast<AFPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			FPSGameMode->ServerNotifyRoundEndComplete(this);
			break;
		}
		case EMatchPhase::AbilityDraft:
		{
			/* Bring up Ability Draft UI and determine */

			AFPSGameState* FPSGameState = Cast<AFPSGameState>(GetWorld()->GetGameState());
			const bool bCanSelect = (GetPlayerState<AFPSPlayerState>() == FPSGameState->CurrentLoserState);
			Client_SetCanSelectUI(bCanSelect);
			Client_ShowDraftScreen();
			break;
		}
	}
}

#pragma region Input
void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::MoveInput);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::LookInput);
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &AFPSPlayerController::JumpInputStarted);
		Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFPSPlayerController::JumpInputCompleted);
		Input->BindAction(PrimaryFireAction, ETriggerEvent::Started, this, &AFPSPlayerController::PrimaryFireInputStarted);
		Input->BindAction(PrimaryFireAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::PrimaryFireInputTriggered);
	}
}

void AFPSPlayerController::MoveInput(const FInputActionValue& Value)
{
	PlayerCharacter->Move(Value);
}

void AFPSPlayerController::LookInput(const FInputActionValue& Value)
{
	PlayerCharacter->Look(Value);
}

void AFPSPlayerController::JumpInputStarted(const FInputActionValue& Value)
{
	PlayerCharacter->JumpStart();
}

void AFPSPlayerController::JumpInputCompleted(const FInputActionValue& Value)
{
	PlayerCharacter->JumpEnd();
}

void AFPSPlayerController::PrimaryFireInputStarted(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("PrimaryFireInputStarted() from FPSPlayerController."));
	PlayerCharacter->PrimaryFire();
}

void AFPSPlayerController::PrimaryFireInputTriggered(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("PrimaryFireInputTriggered() from FPSPlayerController."));
	PlayerCharacter->PrimaryFire();
}

#pragma endregion

#pragma region Local UI
void AFPSPlayerController::Client_ShowDraftScreen_Implementation()
{
	if (AFPSHudController* HUD = Cast<AFPSHudController>(GetHUD()))
	{
		// Destroy controlled pawn.
		if (IsValid(GetPawn()))
		{
			GetPawn()->Destroy();
		}

		HUD->ShowAbilitySelection();
	}
}

void AFPSPlayerController::Client_SetCanSelectUI_Implementation(bool CanSelectUI)
{
	if (CanSelectUI)
	{
		// Set Input Mode to UI Only
		FInputModeUIOnly InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		//InputModeData.SetWidgetToFocus(DraftWidget->TakeWidget());
		SetInputMode(InputModeData);
		SetShowMouseCursor(true);
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
	}
	else
	{
		FInputModeGameOnly InputModeData;
		SetInputMode(InputModeData);
		SetShowMouseCursor(false);
		bEnableClickEvents = false;
		bEnableMouseOverEvents = false;
	}
}

void AFPSPlayerController::Server_FinishedDraft_Implementation()
{
	if (AFPSGameMode* FPSGameMode = GetWorld()->GetAuthGameMode<AFPSGameMode>())
	{
		FPSGameMode->PlayerFinishedDraft(this);
	}
}
#pragma endregion