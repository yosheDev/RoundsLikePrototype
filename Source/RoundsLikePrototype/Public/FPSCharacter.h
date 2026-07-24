// Copyrighted Jacob Jones 2026

#pragma once

// Preprocessor directives
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterWeaponHolder.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Weapons/ProjectileWeapon.h"
#include "Weapons/IWeaponHolder.h"
#include "FPSCharacter.generated.h"

// Forward declarations
class AShooterWeapon;
class UInputAction;
class UInputComponent;
class UGameplayAbility;
class UVitalityAttributeSet;
class UMovementAttributeSet;
class UGunplayAttributeSet;
class UWidgetComponent;

/*
 *  Player character class. Inputs from controller are routed to here and handled.
 *  Manages abilities: Jump, PrimaryFire
 *  Reacts to health attribute changes.
 */

UCLASS(abstract)
class ROUNDSLIKEPROTOTYPE_API AFPSCharacter : public ACharacter, public IAbilitySystemInterface, public IWeaponHolder
{
	GENERATED_BODY()

#pragma region Components
public:
	/** AI Noise emitter component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPawnNoiseEmitterComponent> PawnNoiseEmitter;

protected:

	/** Camera used for the first-person perspective. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> FirstPersonCamera;

	/** Handles camera distance and lag. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> SpringArm;

	/** Handles ability activation and replication. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UFPSAbilitySystemComponent> FPSAbilitySystemComponent;

	/** Displays health above Character head for a duration after being damaged or healed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* HealthWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TSubclassOf<UUserWidget> HealthWidgetClass;

#pragma endregion

#pragma region Abilities
protected:
	/** Iterates through and grants default abilities. */
	void GiveDefaultAbilities();

	/** Observes FPSAbilitySystemComponent::OnAbilityGranted(), responds when an ability is granted to this character. */
	void HandleAbilityGranted(const FGameplayAbilitySpec& Spec);

	/** If granted ability is listed in this function, cache it.Only caches frequently used abilities(such as Jump and Shoot.) */
	void TryCacheAbilitySpecHandle(const FGameplayAbilitySpec& Spec);

	/** Array exposed to the Editor to pick default abilities in Blueprint subclass. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

private:
	// SpecHandles for caching commonly used abilities to avoid constant lookup. Makes activating them O(1)
	FGameplayAbilitySpecHandle JumpAbilityHandle;
	FGameplayAbilitySpecHandle PrimaryFireAbilityHandle;
#pragma endregion

#pragma region Weapons
public:
	/** The default weapon class for this character to use. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<AProjectileWeapon> DefaultWeaponClass;

	/** This characters currently held/equipped weapon. */
	UPROPERTY(EditAnywhere, Category = "Weapons", ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<AProjectileWeapon> CurrentWeapon;

	UFUNCTION()
	void OnRep_CurrentWeapon();

protected:

	/** Name of the first person mesh weapon socket */
	UPROPERTY(EditAnywhere, Category = "Weapons")
	FName FirstPersonWeaponSocket = FName("HandGrip_R");

	/** Name of the third person mesh weapon socket */
	UPROPERTY(EditAnywhere, Category = "Weapons")
	FName ThirdPersonWeaponSocket = FName("HandGrip_R");

#pragma region IWeaponHolder
public:
	/** Handles creation and equipping of weapon. */
	virtual void CreateAndEquipWeapon_Implementation(TSubclassOf<AProjectileWeapon> WeaponClass) override;

	/** Returns the currently equipped weapon. */
	virtual AProjectileWeapon* GetEquippedWeapon_Implementation() const override;
#pragma endregion

#pragma endregion

#pragma region Attributes

protected:
	UPROPERTY()
	TObjectPtr<const UVitalityAttributeSet> VitalityAttributes;

	UPROPERTY()
	TObjectPtr<const UMovementAttributeSet> MovementAttributes;

	UPROPERTY()
	TObjectPtr<const UGunplayAttributeSet> GunplayAttributes;

	// Used for client-side health bar displays. Is corrected anytime actual health is replicated by the server.
	UPROPERTY()
	float PredictedHealth;

protected:
	void OnHealthChanged(const FOnAttributeChangeData& Data);

public:

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastDamageTaken(float Damage);
#pragma endregion

public:
	UPROPERTY(EditAnywhere)
	bool bIsDead;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanInput = false;

protected:
	/** Max distance to use for aim traces */
	UPROPERTY(EditAnywhere, Category = "Aim", meta = (ClampMin = 0, ClampMax = 100000, Units = "cm"))
	float MaxAimDistance = 10000.0f;

public:

	AFPSCharacter();

protected:

	virtual void BeginPlay();

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	/** Handles instantiation of ability system including default abilities and cached ability handles. */
	void InitializeAbilitySystem();

#pragma region Input Functions
public:

	//virtual void Move(Vector2D Value) override;
	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void Look(const FInputActionValue& Value);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void Move(const FInputActionValue& Value);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void JumpStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void JumpEnd();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void PrimaryFire();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void PrimaryFireTriggered();
#pragma endregion

protected:

	/** Called when this character's HP is depleted */
	void Die();

	///** Called to allow Blueprint code to react to this character's death */
	//UFUNCTION(BlueprintImplementableEvent, Category = "Shooter", meta = (DisplayName = "On Death"))
	//void BP_OnDeath();

	/** Called from the respawn timer to destroy this character and force the PC to respawn */
	void OnRespawn();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
};
