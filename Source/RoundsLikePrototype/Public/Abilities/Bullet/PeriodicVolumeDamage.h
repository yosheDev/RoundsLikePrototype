// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PeriodicVolumeDamage.generated.h"

class USphereComponent;
class UGameplayEffect;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API APeriodicVolumeDamage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APeriodicVolumeDamage();

    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USphereComponent> DamageSphere;

    UPROPERTY(EditDefaultsOnly, Category = "Default | Gas Damage")
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default | Gas Damage")
    float VolumeDuration = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Default | Gas Damage",
        meta = (ClampMin = "0.01"))
    float DamageInterval = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Default | Gas Damage", meta = (ClampMin = "0.0"))
    float DamageEffectLevel = 1.0f;

    UPROPERTY()
    TSet<TObjectPtr<AActor>> OverlappingActors;

    FTimerHandle DamageTimerHandle;

    UFUNCTION()
    void OnVolumeBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnVolumeEndOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex);

    void ApplyPeriodicDamage();
    void StartDamageTimerIfNeeded();
    void StopDamageTimerIfEmpty();
};
