// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	Slide = 0
};

class FSavedMove_Custom : public FSavedMove_Character
{
public:
    FVector SavedSlideDirection = FVector::ForwardVector;
    float SavedSlideTimeRemaining = 0.f;
    bool bSavedSliding = false;

    virtual void Clear() override;
    virtual uint8 GetCompressedFlags() const override;
    virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, const FVector& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
    virtual void PrepMoveFor(ACharacter* Character) override;
    virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
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
    float MinimumSlideSpeed = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float MinimumStartSpeed = 450.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideFriction = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Slide")
    float SlideBrakingDeceleration = 180.f;

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

    bool TryStartSlide(const FVector& Direction);
    void StopSlide();

    bool IsSliding() const;

    virtual void UpdateFromCompressedFlags(uint8 Flags) override;

    virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
    virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

private:
    friend class FSavedMove_Custom;

    static constexpr uint8 SlideCustomMode = static_cast<uint8>(ECustomMovementMode::Slide);

    FVector SlideDirection = FVector::ForwardVector;
    float SlideTimeRemaining = 0.0f;

    bool bSlideRequested = false;

    void PhysSlide(float DeltaTime, int32 Iterations);
    bool CanStartSlide() const;
    void SetSlideState(bool bEnable);

    void RestoreSlideState(
        bool bWasSliding,
        const FVector& SavedDirection,
        float SavedTimeRemaining);
};
