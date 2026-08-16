// Copyrighted Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "Abilities/AttributeSets/VitalityAttributeSet.h"
#include "Abilities/AttributeSets/MovementAttributeSet.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ROUNDSLIKEPROTOTYPE_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
	TObjectPtr<UFPSAbilitySystemComponent> FPSAbilitySystemComponent;

	UPROPERTY(Replicated)
	TArray<FGameplayTag> AccruedAbilities;

	/*   Do this when you need the definition from the AccruedAbility
	
	UAbilityDefinition* Definition = AbilityDefinitions::Find(AccruedAbilities[i].AbilityTag);

	*/

	UPROPERTY()
	TObjectPtr<UVitalityAttributeSet> VitalityAttributeSet;

	UPROPERTY()
	TObjectPtr<UMovementAttributeSet> MovementAttributeSet;

	UPROPERTY()
	TObjectPtr<UGunplayAttributeSet> GunplayAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TObjectPtr<UDataTable> VitalityAttributeDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TObjectPtr<UDataTable> MovementAttributeDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TObjectPtr<UDataTable> GunplayAttributeDataTable;

protected:
	UPROPERTY()
	TMap<FString, float> SavedVitalityAttributesMap;

	UPROPERTY()
	TMap<FString, float> SavedMovementAttributesMap;

	UPROPERTY()
	TMap<FString, float> SavedGunplayAttributesMap;

public:
	AFPSPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	virtual void CopyProperties(APlayerState* PlayerState) override;

	void RestoreAttributesAfterTravel();

	void ReapplyAbilitiesAfterTravel();

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_AddAccruedAbility(FGameplayTag AbilityTag);

protected:

	/** Gameplay initialization */
	virtual void BeginPlay();

	/** Gameplay cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason);

};
