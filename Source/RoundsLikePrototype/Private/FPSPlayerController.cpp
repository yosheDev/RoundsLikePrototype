// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "FPSCharacter.h"
#include "RoundsLikePrototype.h"

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController())
	{

	}
}

void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// add the input mapping contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
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