// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MatchEconomyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROUNDSLIKEPROTOTYPE_API UMatchEconomyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMatchEconomyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// The currency used for choosing upgrades / abilities.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Bottlecaps = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint8 MaxBottlecaps = 3;

	UFUNCTION()
	void SetBottlecaps(uint8 Amount);

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAbilityOffers, BlueprintReadOnly)
	TArray<FPrimaryAssetId> CurrentAbilityOffers;

	UFUNCTION()
	void OnRep_CurrentAbilityOffers();

	#pragma region Pure Stat Upgrades
	UPROPERTY(BlueprintReadWrite)
	uint8 UpgradeBulletDamage = 0;

	UPROPERTY(BlueprintReadWrite)
	uint8 UpgradeBulletSpeed = 0;

	UPROPERTY(BlueprintReadWrite)
	uint8 UpgradeBulletArc = 0;

	UPROPERTY(BlueprintReadWrite)
	uint8 UpgradeMaxHealth = 0;

	UPROPERTY(BlueprintReadWrite)
	uint8 UpgradeMovementSpeed = 0;
	#pragma endregion
};
