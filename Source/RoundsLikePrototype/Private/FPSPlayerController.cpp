// Copyrighted Jacob Jones 2026


#include "FPSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "FPSHudController.h"
#include "FPSCharacter.h"
#include "GameFramework/PlayerState.h"
#include "FPSPlayerState.h"
#include "EnhancedInputComponent.h"
#include "FPSGameState.h"
#include "FPSGameMode.h"
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

	PlayerCharacter = Cast<AFPSCharacter>(InPawn);

	SetupMappingContexts();

	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->Tags.Add(PlayerPawnTag);
	}
}

void AFPSPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	
}

void AFPSPlayerController::Client_OnMatchPhaseChanged_Implementation(EMatchPhase NewPhase)
{
	if (!IsLocalController())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("PresentationLog: FPSPlayerController: OnMatchPhaseChanged: [%s]"), *UEnum::GetValueAsString(NewPhase));
	switch (NewPhase)
	{
		case EMatchPhase::RoundStarting:
		{
			// Disable Gameplay Input
			SetInputEnabled(false);
			SetDisplayHUD(true);
			break;
		}
		case EMatchPhase::InRound:
		{
			// Enable Gameplay Input
			SetInputEnabled(true);
			Client_SetCanSelectUI(false);
			SetDisplayHUD(true);
			break;
		}
		case EMatchPhase::RoundEnd:
		{
			/* Perform and finish any animations, sequences, UI, etc. before calling ServerNotifyRoundEndComplete(). */
			
			Server_NotifyRoundEndComplete();
			break;
		}
		case EMatchPhase::AbilityDraft:
		{
			/* Bring up Ability Draft UI*/

			Client_ShowDraftScreen();
			break;
		}
	}
}

#pragma region Input
void AFPSPlayerController::Client_SetInputEnabled_Implementation(bool bIsEnabled)
{
	SetInputEnabled(bIsEnabled);
}

void AFPSPlayerController::SetInputEnabled(bool bIsEnabled)
{
	UE_LOG(LogTemp, Log, TEXT("PresentationLog: [%s] InputEnabled: [%s]"), *GetName(), bIsEnabled ? TEXT("true") : TEXT("false"));
	if (bIsEnabled)
	{
		EnableInput(this);
		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);
	}
	else
	{
		DisableInput(this);
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);
	}
}

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
		Input->BindAction(PrimaryFireAction, ETriggerEvent::Completed, this, &AFPSPlayerController::PrimaryFireInputCompleted);
		Input->BindAction(ShiftAction, ETriggerEvent::Started, this, &AFPSPlayerController::ShiftInputStarted);
		Input->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AFPSPlayerController::ShiftInputCompleted);
	}
}

void AFPSPlayerController::MoveInput(const FInputActionValue& Value)
{
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->Move(Value);
	}
}

void AFPSPlayerController::LookInput(const FInputActionValue& Value)
{
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->Look(Value);
	}
}

void AFPSPlayerController::JumpInputStarted(const FInputActionValue& Value)
{
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->JumpStart();
	}
}

void AFPSPlayerController::JumpInputCompleted(const FInputActionValue& Value)
{
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->JumpEnd();
	}
}

void AFPSPlayerController::PrimaryFireInputStarted(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("PrimaryFireInputStarted() from FPSPlayerController."));
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->PrimaryFire();
	}
}

void AFPSPlayerController::PrimaryFireInputTriggered(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("PrimaryFireInputTriggered() from FPSPlayerController."));
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->PrimaryFire();
	}
}

void AFPSPlayerController::PrimaryFireInputCompleted(const FInputActionValue& Value)
{
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->PrimaryFireCompleted();
	}
}

void AFPSPlayerController::ShiftInputStarted(const FInputActionValue& Value)
{
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->ShiftInput();
	}
}

void AFPSPlayerController::ShiftInputCompleted(const FInputActionValue& Value)
{
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->ShiftInputCompleted();
	}
}
#pragma endregion

#pragma region Server RPCs
void AFPSPlayerController::Server_NotifyRoundEndComplete_Implementation()
{
	if (AFPSGameMode* GM = GetWorld()->GetAuthGameMode<AFPSGameMode>())
	{
		GM->NotifyRoundEndComplete(this);
	}
}

void AFPSPlayerController::Server_NotifyMatchStateDisplayComplete_Implementation()
{
	if (AFPSGameMode* GM = GetWorld()->GetAuthGameMode<AFPSGameMode>())
	{
		GM->NotifyMatchStateDisplayComplete(this);
	}
}

void AFPSPlayerController::Server_NotifyLoadingAnimationFinished_Implementation(EMatchLoadingScreenPhase FinishedPhase)
{
	if (AFPSGameMode* GameMode = GetWorld()->GetAuthGameMode<AFPSGameMode>())
	{
		GameMode->NotifyLoadingAnimationFinished(this, FinishedPhase);
	}
}

void AFPSPlayerController::Server_FinishedDraft_Implementation()
{
	if (AFPSGameMode* FPSGameMode = GetWorld()->GetAuthGameMode<AFPSGameMode>())
	{
		FPSGameMode->PlayerFinishedDraft(this);
	}
}

void AFPSPlayerController::Server_SetHoveredStat_Implementation(int32 WidgetID)
{
	UE_LOG(LogTemp, Log, TEXT("DraftUILog: Player Controller reached."));
	
	if (AFPSGameState* GS = GetWorld()->GetGameState<AFPSGameState>())
	{
		// Ignore request if this Player State is not the current loser.
		AFPSPlayerState* PS = GetPlayerState<AFPSPlayerState>();
		if (GS->CurrentLoserState != PS)
		{
			return;
		}

		GS->EconomyComponent->Server_SetHoveredStat(WidgetID);
		UE_LOG(LogTemp, Log, TEXT("DraftUILog: Call EconomyComponent::SetHoveredStat ID is [%d]"), WidgetID);
	}
}
#pragma endregion

#pragma region Local UI
void AFPSPlayerController::Client_ShowDraftScreen_Implementation()
{
	if (AFPSHudController* HUD = Cast<AFPSHudController>(GetHUD()))
	{
		HUD->ShowAbilitySelection();
	}
}

void AFPSPlayerController::Client_SetCanSelectUI_Implementation(bool CanSelectUI)
{
	if (CanSelectUI)
	{
		//UE_LOG(LogTemp, Log, TEXT("[%s]: YES SELECT UI"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));
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
		//UE_LOG(LogTemp, Log, TEXT("[%s]: NO SELECT UI"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));
		FInputModeGameOnly InputModeData;
		SetInputMode(InputModeData);
		SetShowMouseCursor(false);
		bEnableClickEvents = false;
		bEnableMouseOverEvents = false;
	}
}

void AFPSPlayerController::SetDisplayHUD(bool bShouldDisplay)
{
	if (AFPSHudController* HUD = Cast<AFPSHudController>(GetHUD()))
	{
		HUD->SetDisplayHUD(bShouldDisplay);
	}
}

void AFPSPlayerController::Client_PresentMatchState_Implementation(FGamePlayerData PlayerOneData, FGamePlayerData PlayerTwoData, uint8 CurrentLoserID)
{
	UE_LOG(LogTemp, Log, TEXT("PresentationLog: FPSPlayerController::PresentMatchState()"));

	if (AFPSHudController* HUD = Cast<AFPSHudController>(GetHUD()))
	{
		HUD->PresentMatchState(PlayerOneData, PlayerTwoData, CurrentLoserID);
	}
}
#pragma endregion