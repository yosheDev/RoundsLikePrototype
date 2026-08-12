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
	// Sets default values for this component's properties
	UAmmoComponent();

	bool HasAmmo() const;

	// Server authoritative, returns false if there is no ammo.
	bool TryConsumeAmmo();

	UFUNCTION(BlueprintPure)
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure)
	int32 GetMaxAmmo() const { return MaxAmmo; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ProcessAmmoReturns();

	void ScheduleNextAmmoReturn();

	float GetServerTime() const;

	void AddAmmoReturn(float ReturnTime);

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

	FTimerHandle AmmoReturnTimerHandle;
};
