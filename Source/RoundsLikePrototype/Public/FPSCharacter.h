// Copyrighted Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterWeaponHolder.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Weapons/ProjectileWeapon.h"
#include "Weapons/IWeaponHolder.h"
#include "FPSCharacter.generated.h"

class AShooterWeapon;
class UInputAction;
class UInputComponent;
class UGameplayAbility;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBulletCountUpdatedDelegate, int32, MagazineSize, int32, Bullets);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamagedDelegate, float, LifePercent);

/*
 *  Player character class. Inputs from controller are routed to here and handled.
 *  Manages abilities: Jump, PrimaryFire
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UFPSAbilitySystemComponent> FPSAbilitySystemComponent;

#pragma endregion

#pragma region Abilities
protected:
	// Iterates through and grants default abilities.
	void GiveDefaultAbilities();

	// Observes FPSAbilitySystemComponent::OnAbilityGranted(), responds when an ability is granted to this character.
	void HandleAbilityGranted(const FGameplayAbilitySpec& Spec);

	// If granted ability is listed in this function, cache it. Only caches frequently used abilities(such as Jump and Shoot.)
	void TryCacheAbilitySpecHandle(const FGameplayAbilitySpec& Spec);

	// Array exposed to the Editor to pick default abilities in Blueprint subclass.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

private:
	// SpecHandles for caching commonly used abilities to avoid constant lookup. Makes activating them O(1)
	FGameplayAbilitySpecHandle JumpAbilityHandle;
	FGameplayAbilitySpecHandle PrimaryFireAbilityHandle;
#pragma endregion

#pragma region Weapons
public:
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<AProjectileWeapon> DefaultWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapons")
	TObjectPtr<AProjectileWeapon> CurrentWeapon;
protected:

	/** Name of the first person mesh weapon socket */
	UPROPERTY(EditAnywhere, Category = "Weapons")
	FName FirstPersonWeaponSocket = FName("HandGrip_R");

	/** Name of the third person mesh weapon socket */
	UPROPERTY(EditAnywhere, Category = "Weapons")
	FName ThirdPersonWeaponSocket = FName("HandGrip_R");

#pragma region IWeaponHolder
public:

	virtual void CreateAndEquipWeapon_Implementation(TSubclassOf<AProjectileWeapon> WeaponClass) override;

	virtual AProjectileWeapon* GetEquippedWeapon_Implementation() const override;
#pragma endregion

#pragma endregion

protected:
	/** Max distance to use for aim traces */
	UPROPERTY(EditAnywhere, Category = "Aim", meta = (ClampMin = 0, ClampMax = 100000, Units = "cm"))
	float MaxAimDistance = 10000.0f;

	/** Max HP this character can have */
	UPROPERTY(EditAnywhere, Category = "Health")
	float MaxHP = 500.0f;

	/** Current HP remaining to this character */
	float CurrentHP = 0.0f;

	/** Team ID for this character*/
	UPROPERTY(EditAnywhere, Category = "Team")
	uint8 TeamByte = 0;

	/** Actor tag to grant this character when it dies */
	UPROPERTY(EditAnywhere, Category = "Team")
	FName DeathTag = FName("Dead");

public:

	/** Constructor */
	AFPSCharacter();

protected:

	/** Gameplay initialization */
	virtual void BeginPlay();

	/** Gameplay cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason);

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	void InitializeAbilitySystem();

public:

	/** Handle incoming damage */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

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

	/** Returns true if the character is dead */
	bool IsDead() const;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
};
