// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmoComponent.generated.h"


USTRUCT(BlueprintType)
struct FPendingAmmoReturn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float ReturnTime = 0.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROUNDSLIKEPROTOTYPE_API UAmmoComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	

	UAmmoComponent();

	// Branches into predicted or authoratative.
	bool TryConsumeAmmo();

	UFUNCTION(BlueprintCallable)
	void SetClipCapacity(int32 Amount);

	// Is current ammo greater than 0?
	UFUNCTION(BlueprintCallable)
	bool HasAmmo() const;

	// Sets CurrentAmmo or ClientPredictedAmmo to be equal to amount.
	void SetAmmo(int32 Amount);

	UFUNCTION(BlueprintPure)
	int32 GetCurrentAmmo() const { return CurrentAmmo; }


	UFUNCTION(BlueprintPure)
	int32 GetMaxAmmo() const { return MaxAmmo; }

protected:
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ProcessAmmoReturns();

	void ScheduleNextAmmoReturn();

	void AddAmmoReturn(float ReturnTime);

	bool TryConsumeAmmo_Predicted();
	bool TryConsumeAmmo_Authoratative();

	float GetServerTime() const;
private:

	void UpdateLocalAmmoUI();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing=OnRep_CurrentAmmo)
	int32 CurrentAmmo = -1;

	UFUNCTION()
	void OnRep_CurrentAmmo();

	UPROPERTY(BlueprintReadWrite)
	int32 ClientPredictedAmmo = -1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxAmmo = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AmmoReturnDelay = 2.0f;

	UPROPERTY()
	TArray<float> PendingReturns;

	UPROPERTY(ReplicatedUsing = OnRep_EarliestReturnServerTime)
	float EarliestReturnServerTime = 0.0f;

	UFUNCTION()
	void OnRep_EarliestReturnServerTime();
	FTimerHandle AmmoReturnTimerHandle;
};
