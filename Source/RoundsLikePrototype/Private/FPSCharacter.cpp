// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "FPSPlayerState.h"
#include "ShooterWeapon.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"

AFPSCharacter::AFPSCharacter()
{
	#pragma region Construct Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bDoCollisionTest = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FirstPersonCamera->bUsePawnControlRotation = false;
	#pragma endregion

	// create the noise emitter component
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));

	// configure movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	FPSAbilitySystemComponent = Cast<UFPSAbilitySystemComponent>(GetAbilitySystemComponent());
	if (FPSAbilitySystemComponent)
	{
		GiveDefaultAbilities();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("FPSAbilitySystemComponent failed to initialize. Is BeginPlay() of the pawn happening before BeginPlay() of the PlayerState?"));
	}
	

	// reset HP to max
	CurrentHP = MaxHP;
}

void AFPSCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the respawn timer
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimer);
}

float AFPSCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// ignore if already dead
	if (CurrentHP <= 0.0f)
	{
		return 0.0f;
	}

	// Reduce HP
	CurrentHP -= Damage;

	// Have we depleted HP?
	if (CurrentHP <= 0.0f)
	{
		Die();
	}

	return Damage;
}

void AFPSCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>() * 1.0f; // Replace the constant float with look sensitivity when that value has a home.

	// only route inputs if the character is not dead
	if (!IsDead())
	{
		AddControllerYawInput(LookVector.X);

		AddControllerPitchInput(-LookVector.Y);
	}
}

void AFPSCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (!IsDead())
	{
		const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

		const FVector ForwardDirection = YawRotation.Vector();
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFPSCharacter::JumpStart()
{
	// only route inputs if the character is not dead
	if (!IsDead())
	{
		FPSAbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("GameplayAbility.Movement.Jump"))));
	}
}

void AFPSCharacter::JumpEnd()
{
	// only route inputs if the character is not dead
	if (!IsDead())
	{
		FGameplayTagContainer JumpTags;
		JumpTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("GameplayAbility.Movement.Jump")));

		FPSAbilitySystemComponent->CancelAbilities(&JumpTags);
	}
}

void AFPSCharacter::DoStartFiring()
{
	// fire the current weapon
	if (CurrentWeapon && !IsDead())
	{
		CurrentWeapon->StartFiring();
	}
}

void AFPSCharacter::DoStopFiring()
{
	// stop firing the current weapon
	if (CurrentWeapon && !IsDead())
	{
		CurrentWeapon->StopFiring();
	}
}

void AFPSCharacter::DoSwitchWeapon()
{
	// ensure we have at least two weapons two switch between
	if (OwnedWeapons.Num() > 1 && !IsDead())
	{
		// deactivate the old weapon
		CurrentWeapon->DeactivateWeapon();

		// find the index of the current weapon in the owned list
		int32 WeaponIndex = OwnedWeapons.Find(CurrentWeapon);

		// is this the last weapon?
		if (WeaponIndex == OwnedWeapons.Num() - 1)
		{
			// loop back to the beginning of the array
			WeaponIndex = 0;
		}
		else {
			// select the next weapon index
			++WeaponIndex;
		}

		// set the new weapon as current
		CurrentWeapon = OwnedWeapons[WeaponIndex];

		// activate the new weapon
		CurrentWeapon->ActivateWeapon();
	}
}

AShooterWeapon* AFPSCharacter::FindWeaponOfType(TSubclassOf<AShooterWeapon> WeaponClass) const
{
	// check each owned weapon
	for (AShooterWeapon* Weapon : OwnedWeapons)
	{
		if (Weapon->IsA(WeaponClass))
		{
			return Weapon;
		}
	}

	// weapon not found
	return nullptr;

}

void AFPSCharacter::Die()
{
	// deactivate the weapon
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->DeactivateWeapon();
	}

	// grant the death tag to the character
	Tags.Add(DeathTag);

	// stop character movement
	GetCharacterMovement()->StopMovementImmediately();

	// disable controls
	DisableInput(nullptr);

	// call the BP handler
	//BP_OnDeath();

	// schedule character respawn
	//GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AFPSCharacter::OnRespawn, RespawnTime, false);
}

void AFPSCharacter::OnRespawn()
{
	// destroy the character to force the PC to respawn
	Destroy();
}

bool AFPSCharacter::IsDead() const
{
	// the character is dead if their current HP drops to zero
	return CurrentHP <= 0.0f;
}

UAbilitySystemComponent* AFPSCharacter::GetAbilitySystemComponent() const
{
	// Return the Ability System Component on the FPSPlayerState.
	AFPSPlayerState* FPSPlayerState = Cast<AFPSPlayerState>(GetPlayerState());
	return FPSPlayerState->FPSAbilitySystemComponent;
}

void AFPSCharacter::GiveDefaultAbilities()
{
	// Safety checks: execute ONLY on the server and if the ASC is valid
	if (GetLocalRole() != ROLE_Authority || !FPSAbilitySystemComponent)
	{
		return;
	}

	for (TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (AbilityClass)
		{
			// Grant the ability
			FPSAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, -1, this));
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Added an ability"));
		}
	}
}
