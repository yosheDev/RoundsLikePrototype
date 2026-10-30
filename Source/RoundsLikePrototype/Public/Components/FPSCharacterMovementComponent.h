// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/CharacterMovementReplication.h"
#include "FPSCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	Slide = 0
};

class FSavedMove_Custom : public FSavedMove_Character
{
public:
    bool bSavedSliding = false; // deprecated soon probably?
    FVector SavedSlideDirection = FVector::ForwardVector; // deprecated soon probably?

    float SavedSlideTimeRemaining = 0.f;
    bool bSavedWantsToStartSlide = false;
    FVector SavedSlideStartDirection = FVector::ForwardVector;
    EMovementMode SavedMovementMode = MOVE_Walking;
    uint8 SavedCustomMovementMode = 0;

    virtual void Clear() override;
    virtual uint8 GetCompressedFlags() const override;
    virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, const FVector& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
    virtual void PrepMoveFor(ACharacter* Character) override;
    virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
};

struct FSlideNetworkMoveData : public FCharacterNetworkMoveData
{
    using Super = FCharacterNetworkMoveData;

    bool bWantsToStartSlide = false;
    FVector_NetQuantizeNormal SlideStartDirection = FVector::ForwardVector;

    virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;

    virtual bool Serialize(
        UCharacterMovementComponent& CharacterMovement,
        FArchive& Ar,
        UPackageMap* PackageMap,
        ENetworkMoveType MoveType) override;
};

struct FSlideNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
    FSlideNetworkMoveDataContainer()
    {
        NewMoveData = &MoveData[0];
        PendingMoveData = &MoveData[1];
        OldMoveData = &MoveData[2];
    }

    FSlideNetworkMoveData MoveData[3];
};

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UFPSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
    UFPSCharacterMovementComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideDuration = 0.85f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideSpeed = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideBrakingDeceleration = 180.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideFriction = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideSteeringAcceleration = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float MaximumSlideSpeed = 1400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideCapsuleHalfHeight = 55.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideEnterHalfHeight = 44.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideExitHalfHeight = 88.f;

    UFUNCTION(BlueprintCallable, Category = "Movement|Slide")
    void RequestSlide(const FVector& Direction);

    UFUNCTION(BlueprintCallable, Category = "Movement|Slide")
    bool TryStartSlide(const FVector& Direction);

    UFUNCTION(BlueprintCallable, Category = "Movement|Slide")
    void StopSlide();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement|Slide")
    bool IsSliding() const;

    virtual void UpdateFromCompressedFlags(uint8 Flags) override;

    virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
    virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

    virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

    // Override the network smoothing so that instantaneous bursts of speed can have visual clarity instead of lagging behind.
    virtual void SmoothClientPosition(float DeltaSeconds) override;
private:
    friend class FSavedMove_Custom;
    FSlideNetworkMoveDataContainer SlideNetworkMoveDataContainer;

    static constexpr uint8 SlideCustomMode = static_cast<uint8>(ECustomMovementMode::Slide);

    float SlideTimeRemaining = 0.0f;

    bool bPendingSlideStartRequest = false;
    FVector PendingSlideStartDirection = FVector::ForwardVector;

    //UPROPERTY(Replicated)
    FVector SlideDirection = FVector::ForwardVector;

    UPROPERTY(EditAnywhere, Category = "Movement|Slide|Networking", meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float SlideMeshCatchupSpeedMultiplier = 1.0f;

    void PhysSlide(float DeltaTime, int32 Iterations);
    bool CanStartSlide() const;
    void SetSlideState(bool bEnable);

    void RestoreSlideState(
        bool bWasSliding,
        const FVector& SavedDirection,
        float SavedTimeRemaining);
};
