// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Components/HealthComponent.h"
#include "Abilities/AttributeSets/VitalityAttributeSet.h"
#include "DestructableActor.generated.h"

UCLASS()
class ROUNDSLIKEPROTOTYPE_API ADestructableActor : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UVitalityAttributeSet* VitalityAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;
#pragma endregion

public:	

	ADestructableActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|Vitality")
	float MaxHealth = 50.0f;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Vitality")
	void OnDamage(float DamageAmount, float NewHealth);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Vitality")
	void OnDestroy();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	virtual void BeginPlay() override;
};
