// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BottlecapAllocationData.h"
#include "MatchEconomyComponent.generated.h"

struct FBottlecapReturnLocation;

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
	uint8 AvailableBottlecaps = 3;

	UPROPERTY()
	TArray<uint8> AvailableBottlecapIndices;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint8 MaxBottlecaps = 3;

	// Index = ID(index) of bottlecap in DraftingUI
	UPROPERTY()
	TArray<bool> BottlecapAllocations;

	UFUNCTION()
	void InitializeBottlecapAllocations();

	UFUNCTION()
	void SetBottlecaps(uint8 Amount);

	UFUNCTION()
	bool CanAllocateBottlecaps(uint8 Amount);

	UFUNCTION()
	void AllocateBottlecaps(uint8 Amount, int32 WidgetID, const TArray<FBottlecapReturnLocation>& AllocationLocations);

	UFUNCTION(Server, Reliable)
	void Server_AllocateBottlecaps(uint8 Amount, int32 WidgetID, const TArray<FBottlecapReturnLocation>& AllocationLocations);

	UFUNCTION()
	void DeallocateBottlecaps(int32 WidgetID);

	UFUNCTION(Server, Reliable)
	void Server_DeallocateBottlecaps(int32 WidgetID);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateBottlecapHUD(uint8 BottlecapID, FBottlecapReturnLocation AllocationLocation, bool bIsDeallocating = false);

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
	
protected:

	// Used for lookups with WidgetID when deallocating.
	UPROPERTY()
	TMap<int32, FBottlecapAllocationData> AllocationData;

private:

	UPROPERTY()
	APlayerController* PC;

	UFUNCTION()
	APlayerController* GetPlayerController();
};
