// Copyrighted Jacob Jones 2026

#pragma region Preprocessor Directives
#pragma region Custom Includes
#include "FPSCharacter.h"
#include "FPSPlayerState.h"
#include "UI/HUD/HealthBar.h"
#include "ShooterWeapon.h"
#pragma endregion

#pragma region Unreal Includes
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#pragma endregion

#pragma region Components
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/WidgetComponent.h"
#pragma endregion
#pragma endregion

// Constructor
AFPSCharacter::AFPSCharacter()
{
	#pragma region Construct Components
	#pragma region Construct Camera
	/* Spring Arm */
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bDoCollisionTest = false;

	/* FP Camera */
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FirstPersonCamera->bUsePawnControlRotation = false;
	#pragma endregion

	/* Health Bar */
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget->SetupAttachment(GetCapsuleComponent());

	// Create Noise Emitter
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));
	#pragma endregion

	bReplicates = true;
	NetUpdateFrequency = 100.0f;
	MinNetUpdateFrequency = 30.0f;
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Assign HealthWidgetClass default from Blueprint class.
	/*if (HealthWidgetClass)
	{
		HealthWidget->SetWidgetClass(HealthWidgetClass);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthWidgetClass is null on [%s]"), *GetName());
	}*/

	#pragma region Instantiate Weapon
	if (HasAuthority())
	{
		CreateAndEquipWeapon_Implementation(DefaultWeaponClass);
	}
	#pragma endregion
}

void AFPSCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSCharacter, CurrentWeapon);
}

void AFPSCharacter::PossessedBy(AController* NewController)
{
	/* PossessedBy() runs when controller possesses this character. Only runs on the server.*/
	Super::PossessedBy(NewController);

	if (!FPSAbilitySystemComponent)
	{
		InitializeAbilitySystem();
	}
}

void AFPSCharacter::OnRep_PlayerState()
{
	/* Only runs on clients. */
	Super::OnRep_PlayerState();

	if (!FPSAbilitySystemComponent)
	{
		InitializeAbilitySystem();
	}
}

#pragma region Input Actions
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
		//FPSAbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("GameplayAbility.Movement.Jump"))));
		//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("%s cached Jump Handle: %s"), *GetName(), JumpAbilityHandle.IsValid() ? TEXT("true") : TEXT("false")));
		
		bool bActivated = FPSAbilitySystemComponent->TryActivateAbility(JumpAbilityHandle);

		//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("Did jump ability activate: %s"), bActivated ? TEXT("true") : TEXT("false")));
	}
}

void AFPSCharacter::JumpEnd()
{
	// only route inputs if the character is not dead
	if (!IsDead())
	{
		//FGameplayTagContainer JumpTags;
		//JumpTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("GameplayAbility.Movement.Jump")));
		//FPSAbilitySystemComponent->CancelAbilities(&JumpTags);
		FPSAbilitySystemComponent->CancelAbilityHandle(JumpAbilityHandle);
	}
}

#pragma region Weapon Input Handling
void AFPSCharacter::PrimaryFire()
{
	// only route inputs if the character is not dead
	if (!IsDead())
	{
		//FPSAbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("GameplayAbility.Movement.Jump"))));
		//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("%s cached Primary Fire Handle: %s"), *GetName(), JumpAbilityHandle.IsValid() ? TEXT("true") : TEXT("false")));

		bool bActivated = FPSAbilitySystemComponent->TryActivateAbility(PrimaryFireAbilityHandle);

		//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("Did primary fire activate: %s"), bActivated ? TEXT("true") : TEXT("false")));
	}
}

void AFPSCharacter::PrimaryFireTriggered()
{
	PrimaryFire();
}
#pragma endregion

#pragma endregion

#pragma region Weapon

#pragma region IWeaponHolder
void AFPSCharacter::CreateAndEquipWeapon_Implementation(TSubclassOf<AProjectileWeapon> WeaponClass)
{
	AProjectileWeapon* Weapon = GetWorld()->SpawnActor<AProjectileWeapon>(WeaponClass);

	// Validate the weapon actually spawned.
	if (!Weapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn weapon"));
		return;
	}

	Weapon->SetOwner(this);
	Weapon->SetInstigator(this);

	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FirstPersonWeaponSocket);

	CurrentWeapon = Weapon;
}

AProjectileWeapon* AFPSCharacter::GetEquippedWeapon_Implementation() const
{
	return CurrentWeapon;
}
#pragma endregion

#pragma endregion
#pragma region RandomCrapToCleanUp

void AFPSCharacter::Die()
{
	// stop character movement
	GetCharacterMovement()->StopMovementImmediately();

	// disable controls
	DisableInput(nullptr);

	// call the BP handler
	//BP_OnDeath();
}

void AFPSCharacter::OnRespawn()
{
	// destroy the character to force the PC to respawn
	Destroy();
}

bool AFPSCharacter::IsDead() const
{
	// the character is dead if their current HP drops to zero
	return VitalityAttributes->GetHealth() <= 0.0f;
}
#pragma endregion

#pragma region FPS Ability System Component

UAbilitySystemComponent* AFPSCharacter::GetAbilitySystemComponent() const
{
	// Return the Ability System Component on the FPSPlayerState.
	AFPSPlayerState* FPSPlayerState = Cast<AFPSPlayerState>(GetPlayerState());

	if (!FPSPlayerState)
	{
		return nullptr;
	}

	return FPSPlayerState->FPSAbilitySystemComponent;
}

void AFPSCharacter::InitializeAbilitySystem()
{
	/*
		Only runs when FPSPlayerState is guarenteed to exist. (Server->PossessedBy() | Client->OnRep_PlayerState()
		Handles initialization of the FPSAbilitySystemComponent, which is owned by the FPSPlayerState.
	*/
	AFPSPlayerState* FPSPlayerState = GetPlayerState<AFPSPlayerState>();

	if (FPSPlayerState)
	{
		FPSAbilitySystemComponent = FPSPlayerState->FPSAbilitySystemComponent;

		if (FPSAbilitySystemComponent)
		{
			#pragma region Get Network Role String
			ENetRole LocalRole = GetLocalRole();
			FString RoleString = TEXT("Unknown");

			switch (LocalRole)
			{
			case ROLE_Authority:
				RoleString = (GetWorld()->IsNetMode(NM_Client)) ? TEXT("Server (Autonomous)") : TEXT("Server (Authority)");
				break;
			case ROLE_AutonomousProxy:
				RoleString = TEXT("Client (Autonomous Proxy)");
				break;
			case ROLE_SimulatedProxy:
				RoleString = TEXT("Client (Simulated Proxy)");
				break;
			case ROLE_None:
				RoleString = TEXT("None");
				break;
			}
			#pragma endregion
			//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("%s InitiateAbilitySystem()"), *RoleString));

			// Initialize local actor info for the ASC.
			FPSAbilitySystemComponent->InitAbilityActorInfo(FPSPlayerState, this);

			// Bind OnAbilitySpecRecieved to HandleAbilityGranted().
			FPSAbilitySystemComponent->OnAbilitySpecRecieved.AddUObject(this, &AFPSCharacter::HandleAbilityGranted);

			// Initialize Attribute Set References
			VitalityAttributes = FPSAbilitySystemComponent->GetSet<UVitalityAttributeSet>();
			MovementAttributes = FPSAbilitySystemComponent->GetSet<UMovementAttributeSet>();
			GunplayAttributes = FPSAbilitySystemComponent->GetSet<UGunplayAttributeSet>();

			// Initialize Predicted Health
			PredictedHealth = VitalityAttributes->GetHealth();

			// Bind Vitality Health changing with OnHealthChanged(). This is authoritative and only happens when server replicates.
			FPSAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UVitalityAttributeSet::GetHealthAttribute()).AddUObject(this,&AFPSCharacter::OnHealthChanged);

			// Server grants default abilities to character.
			if (GetLocalRole() == ROLE_Authority)
			{
				GiveDefaultAbilities();
			}
			else
			{
				for (const FGameplayAbilitySpec& Spec : FPSAbilitySystemComponent->GetActivatableAbilities())
				{
					TryCacheAbilitySpecHandle(Spec);
				}
			}
		}
	}
}

void AFPSCharacter::GiveDefaultAbilities()
{
	// Execute ONLY on the server and if the ASC is valid, just in case.
	if (GetLocalRole() != ROLE_Authority || !FPSAbilitySystemComponent)
	{
		return;
	}

	// Iterate through all default abilities assigned, and grant them to the character.
	for (TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (AbilityClass)
		{
			// Grant the ability
			FGameplayAbilitySpecHandle AbilityHandle = FPSAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, -1, this));
			const FGameplayAbilitySpec* GrantedSpec = FPSAbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle);

			#pragma region Get Network Role String
			ENetRole LocalRole = GetLocalRole();
			FString RoleString = TEXT("Unknown");

			switch (LocalRole)
			{
			case ROLE_Authority:
				RoleString = (GetWorld()->IsNetMode(NM_Client)) ? TEXT("Server (Autonomous)") : TEXT("Server (Authority)");
				break;
			case ROLE_AutonomousProxy:
				RoleString = TEXT("Client (Autonomous Proxy)");
				break;
			case ROLE_SimulatedProxy:
				RoleString = TEXT("Client (Simulated Proxy)");
				break;
			case ROLE_None:
				RoleString = TEXT("None");
				break;
			}
			#pragma endregion
			//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("%s Granted Ability: %s"), *RoleString, *GrantedSpec->Ability->GetName()));
			// If the ability is the jump ability, cache it for future use.
			

			if (GrantedSpec)
			{
				// Cache any ability specs that will be constantly used, such as jumping and shooting.
				TryCacheAbilitySpecHandle(*GrantedSpec);
			}
		}
	}
}

void AFPSCharacter::HandleAbilityGranted(const FGameplayAbilitySpec& Spec)
{
	/*
		This function listens to FPSAbilitySystemComponent::OnAbilityGranted() delegate. Only runs on the server.
	*/
	#pragma region Get Network Role String
	ENetRole LocalRole = GetLocalRole();
	FString RoleString = TEXT("Unknown");

	switch (LocalRole)
	{
	case ROLE_Authority:
		RoleString = (GetWorld()->IsNetMode(NM_Client)) ? TEXT("Server (Autonomous)") : TEXT("Server (Authority)");
		break;
	case ROLE_AutonomousProxy:
		RoleString = TEXT("Client (Autonomous Proxy)");
		break;
	case ROLE_SimulatedProxy:
		RoleString = TEXT("Client (Simulated Proxy)");
		break;
	case ROLE_None:
		RoleString = TEXT("None");
		break;
	}
	#pragma endregion
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("%s Trying to handle a granted ability: %s"), *RoleString, *(Spec.Ability->GetName())));
	
	TryCacheAbilitySpecHandle(Spec);
}

void AFPSCharacter::TryCacheAbilitySpecHandle(const FGameplayAbilitySpec& Spec)
{
	/* Contains all logic for testing handles for specific actions and caching them. */
	if (Spec.Ability && Spec.Ability->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("GameplayAbility.Movement.Jump"))))
	{
		
		FString AbilityName = Spec.Ability->GetName();
		FString AbilityClassName = Spec.Ability->GetClass()->GetName();

		JumpAbilityHandle = Spec.Handle;

		#pragma region Get Network Role String
		ENetRole LocalRole = GetLocalRole();
		FString RoleString = TEXT("Unknown");

		switch (LocalRole)
		{
		case ROLE_Authority:
			RoleString = (GetWorld()->IsNetMode(NM_Client)) ? TEXT("Server (Autonomous)") : TEXT("Server (Authority)");
			break;
		case ROLE_AutonomousProxy:
			RoleString = TEXT("Client (Autonomous Proxy)");
			break;
		case ROLE_SimulatedProxy:
			RoleString = TEXT("Client (Simulated Proxy)");
			break;
		case ROLE_None:
			RoleString = TEXT("None");
			break;
		}
		#pragma endregion
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, FString::Printf(TEXT("%s Cached Ability Handle for: %s"), *RoleString, *Spec.Ability->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, FString::Printf(TEXT("%s Is JumpAbilityHandle valid after?: %s"), *RoleString, JumpAbilityHandle.IsValid() ? TEXT("true") : TEXT("false")));
		
		const FGameplayAbilitySpec* CachedSpec = FPSAbilitySystemComponent->FindAbilitySpecFromHandle(JumpAbilityHandle);

		GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Red,
			FString::Printf(
				TEXT("Cached: %s | Found: %s"),
				*Spec.Ability->GetName(),
				CachedSpec ? *CachedSpec->Ability->GetName() : TEXT("NULL")
			)
		);

	}
	else if (Spec.Ability && Spec.Ability->AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("GameplayAbility.Weapon.PrimaryFire"))))
	{

		FString AbilityName = Spec.Ability->GetName();
		FString AbilityClassName = Spec.Ability->GetClass()->GetName();

		PrimaryFireAbilityHandle = Spec.Handle;

		#pragma region Get Network Role String
		ENetRole LocalRole = GetLocalRole();
		FString RoleString = TEXT("Unknown");

		switch (LocalRole)
		{
		case ROLE_Authority:
			RoleString = (GetWorld()->IsNetMode(NM_Client)) ? TEXT("Server (Autonomous)") : TEXT("Server (Authority)");
			break;
		case ROLE_AutonomousProxy:
			RoleString = TEXT("Client (Autonomous Proxy)");
			break;
		case ROLE_SimulatedProxy:
			RoleString = TEXT("Client (Simulated Proxy)");
			break;
		case ROLE_None:
			RoleString = TEXT("None");
			break;
		}
		#pragma endregion
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, FString::Printf(TEXT("%s Cached PrimaryFire Ability Handle for: %s"), *RoleString, *Spec.Ability->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, FString::Printf(TEXT("%s Is PrimaryFire Ability Handle valid after?: %s"), *RoleString, PrimaryFireAbilityHandle.IsValid() ? TEXT("true") : TEXT("false")));

		const FGameplayAbilitySpec* CachedSpec = FPSAbilitySystemComponent->FindAbilitySpecFromHandle(PrimaryFireAbilityHandle);

		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Cached: %s | Found: %s"), *Spec.Ability->GetName(), CachedSpec ? *CachedSpec->Ability->GetName() : TEXT("NULL")));
	}
}

#pragma endregion

// Authoratative Update Health
void AFPSCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	/* 
	  This function is called when the server replicates Health updates.
	  It is authoratative and should be used to correct client predictions.
	*/
	float OldHealth = Data.OldValue;
	float NewHealth = Data.NewValue;
	PredictedHealth = Data.NewValue;
	float DamagedAmount = OldHealth - NewHealth;
	//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("[%s] Take Damage [%f]"), *GetName(), DamagedAmount));

	// Update Health Bar
	if (auto* Widget = Cast<UHealthBar>(HealthWidget->GetUserWidgetObject()))
	{
		Widget->UpdateHealthBar(NewHealth, VitalityAttributes->GetMaxHealth());
	}
}

// Multicast RPC called from server when server knows damage is taken.
void AFPSCharacter::MulticastDamageTaken_Implementation(float Damage)
{
	if (GetLocalRole() != ROLE_Authority)
	{

		if (auto* Widget = Cast<UHealthBar>(HealthWidget->GetUserWidgetObject()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Green, FString::Printf(TEXT("[%s] Multicast Damage [%f]"), *GetName(), Damage));
			Widget->UpdateHealthBar(PredictedHealth - Damage, VitalityAttributes->GetMaxHealth());
			PredictedHealth = FMath::Clamp(PredictedHealth - Damage, 0.0f, TNumericLimits<float>::Max());
		}
	}
}