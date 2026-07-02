// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "FPSCharacter.h"
#include "EnhancedInputComponent.h"
#include "RoundsLikePrototype.h"

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
	// reset the bullet counter HUD
	/*if (IsValid(BulletCounterUI))
	{
		BulletCounterUI->BP_UpdateBulletCounter(0, 0);
	}*/

	// find the player start
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		// select a random player start
		AActor* RandomPlayerStart = ActorList[FMath::RandRange(0, ActorList.Num() - 1)];

		// spawn a character at the player start
		const FTransform SpawnTransform = RandomPlayerStart->GetActorTransform();

		if (AFPSCharacter* RespawnedCharacter = GetWorld()->SpawnActor<AFPSCharacter>(CharacterClass, SpawnTransform))
		{
			// possess the character
			Possess(RespawnedCharacter);
		}
	}
}

void AFPSPlayerController::OnPawnDamaged(float LifePercent)
{

}

#pragma region Input
void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::MoveInput);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::LookInput);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::JumpInput);
		Input->BindAction(PrimaryFireAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::PrimaryFireInput);
	}
}

void AFPSPlayerController::MoveInput(const FInputActionValue& Value)
{
	FString DebugMessage = FString::Printf(TEXT("MoveInput() from FPSPlayerController | %s"), *(Value.Get<FVector2D>()).ToString());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DebugMessage);
}

void AFPSPlayerController::LookInput(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("LookInput() from FPSPlayerController."));
}

void AFPSPlayerController::JumpInput(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("JumpInput() from FPSPlayerController."));
}

void AFPSPlayerController::PrimaryFireInput(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("PrimaryFireInput() from FPSPlayerController."));
}

#pragma endregion