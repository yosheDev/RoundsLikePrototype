// Copyright Jacob Jones 2026

#include "Components/FPSCharacterMovementComponent.h"
#include "FPSCharacter.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace SlideMove
{
    constexpr uint8 SlideFlag = FSavedMove_Character::FLAG_Custom_0;
}

class FNetworkPredictionData_Client_Slide : public FNetworkPredictionData_Client_Character
{
public:
    explicit FNetworkPredictionData_Client_Slide(const UFPSCharacterMovementComponent& ClientMovement) : FNetworkPredictionData_Client_Character(ClientMovement)
    {

    }

    virtual FSavedMovePtr AllocateNewMove() override
    {
        return FSavedMovePtr(new FSavedMove_Custom());
    }
};

#pragma region FSavedMove_Character Custom
void FSavedMove_Custom::Clear()
{
    FSavedMove_Character::Clear();

    SavedSlideDirection = FVector::ForwardVector;
    SavedSlideTimeRemaining = 0.f;
    bSavedSliding = false;
}

uint8 FSavedMove_Custom::GetCompressedFlags() const
{
    uint8 Result = FSavedMove_Character::GetCompressedFlags();

    if (bSavedSliding)
    {
        Result |= FLAG_Custom_0;
    }

    return Result;
}

void FSavedMove_Custom::SetMoveFor(
    ACharacter* Character,
    float InDeltaTime,
    const FVector& NewAccel,
    FNetworkPredictionData_Client_Character& ClientData)
{
    FSavedMove_Character::SetMoveFor(
        Character,
        InDeltaTime,
        NewAccel,
        ClientData);

    if (!Character)
    {
        return;
    }

    if (const UFPSCharacterMovementComponent* MoveComp =
        Cast<UFPSCharacterMovementComponent>(
            Character->GetCharacterMovement()))
    {
        bSavedSliding = MoveComp->IsSliding();
        SavedSlideDirection = MoveComp->SlideDirection;
        SavedSlideTimeRemaining = MoveComp->SlideTimeRemaining;
    }
}

void FSavedMove_Custom::PrepMoveFor(ACharacter* Character)
{
    FSavedMove_Character::PrepMoveFor(Character);

    if (!Character)
    {
        return;
    }

    if (UFPSCharacterMovementComponent* MoveComp =
        Cast<UFPSCharacterMovementComponent>(
            Character->GetCharacterMovement()))
    {
        MoveComp->RestoreSlideState(
            bSavedSliding,
            SavedSlideDirection,
            SavedSlideTimeRemaining);
    }
}

bool FSavedMove_Custom::CanCombineWith(
    const FSavedMovePtr& NewMove,
    ACharacter* Character,
    float MaxDelta) const
{
    const FSavedMove_Custom* Other =
        static_cast<const FSavedMove_Custom*>(NewMove.Get());

    if (!Other)
    {
        return false;
    }

    if (bSavedSliding != Other->bSavedSliding ||
        !SavedSlideDirection.Equals(
            Other->SavedSlideDirection, 0.01f))
    {
        return false;
    }

    return FSavedMove_Character::CanCombineWith(
        NewMove,
        Character,
        MaxDelta);
}
#pragma endregion

UFPSCharacterMovementComponent::UFPSCharacterMovementComponent()
{
    // These settings are inherited from CMC. Adjust them
    // in your project as needed.
    bOrientRotationToMovement = true;
}

bool UFPSCharacterMovementComponent::IsSliding() const
{
    return MovementMode == MOVE_Custom && CustomMovementMode == SlideCustomMode;
}

bool UFPSCharacterMovementComponent::CanStartSlide() const
{
    return CharacterOwner
        && UpdatedComponent
        && IsMovingOnGround()
        && !IsSliding()
        && Velocity.Size2D() >= MinimumStartSpeed;
}

bool UFPSCharacterMovementComponent::TryStartSlide(const FVector& Direction)
{
    UE_LOG(LogTemp, Log, TEXT("SlideLog: TryStartSlide"));

    if (!CanStartSlide())
    {
        return false;
    }

    FVector NewDirection = Direction.GetSafeNormal2D();

    if (NewDirection.IsNearlyZero())
    {
        NewDirection = Velocity.GetSafeNormal2D();
    }

    if (NewDirection.IsNearlyZero())
    {
        return false;
    }

    const float InitialSpeed = FMath::Max(Velocity.Size2D(), MinimumStartSpeed);

    Velocity.X = NewDirection.X * InitialSpeed;
    Velocity.Y = NewDirection.Y * InitialSpeed;
    Velocity.Z = 0.f;

    SlideDirection = NewDirection;
    SlideTimeRemaining = SlideDuration;
    bSlideRequested = true;

    SetMovementMode(MOVE_Custom, SlideCustomMode);

    if (CharacterOwner)
    {
        CharacterOwner->Crouch();
    }

    return true;
}

void UFPSCharacterMovementComponent::StopSlide()
{
    if (!IsSliding())
    {
        bSlideRequested = false;
        return;
    }

    bSlideRequested = false;

    // Recheck the floor before selecting the next mode.
    FindFloor(
        UpdatedComponent->GetComponentLocation(),
        CurrentFloor,
        false);

    if (CurrentFloor.IsWalkableFloor())
    {
        SetMovementMode(MOVE_Walking);
    }
    else
    {
        SetMovementMode(MOVE_Falling);
    }

    if (CharacterOwner)
    {
        CharacterOwner->UnCrouch();
    }
}

void UFPSCharacterMovementComponent::RestoreSlideState(
    bool bWasSliding,
    const FVector& SavedDirection,
    float SavedTimeRemaining)
{
    SlideDirection = SavedDirection;
    SlideTimeRemaining = SavedTimeRemaining;
    bSlideRequested = bWasSliding;

    if (bWasSliding)
    {
        if (!IsSliding())
        {
            SetMovementMode(MOVE_Custom, SlideCustomMode);
        }
    }
    else if (IsSliding())
    {
        // Do not force walking here. The movement simulation
        // will determine whether walking or falling is valid.
        SetMovementMode(MOVE_Walking);
    }
}

void UFPSCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
    Super::UpdateFromCompressedFlags(Flags);

    const bool bWantsSlide =
        (Flags & SlideMove::SlideFlag) != 0;

    if (bWantsSlide && !IsSliding())
    {
        // The saved move communicates slide intent. Validate
        // it independently rather than trusting the client.
        FVector Direction = Velocity.GetSafeNormal2D();

        if (Direction.IsNearlyZero())
        {
            Direction = Acceleration.GetSafeNormal2D();
        }

        if (!TryStartSlide(Direction))
        {
            bSlideRequested = false;
        }
    }
}

FNetworkPredictionData_Client* UFPSCharacterMovementComponent::GetPredictionData_Client() const
{
    check(PawnOwner != nullptr);

    if (!ClientPredictionData)
    {
        UFPSCharacterMovementComponent* MutableThis = const_cast<UFPSCharacterMovementComponent*>(this);

        MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Slide(*this);
    }

    return ClientPredictionData;
}

void UFPSCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

    if (PreviousMovementMode == MOVE_Custom &&
        PreviousCustomMode == SlideCustomMode &&
        !IsSliding())
    {
        bSlideRequested = false;

        if (CharacterOwner)
        {
            CharacterOwner->UnCrouch();
        }
    }
}

void UFPSCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
    if (CustomMovementMode == SlideCustomMode)
    {
        PhysSlide(DeltaTime, Iterations);
        return;
    }

    Super::PhysCustom(DeltaTime, Iterations);
}

void UFPSCharacterMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
    if (DeltaTime < MIN_TICK_TIME || !UpdatedComponent || !CharacterOwner)
    {
        return;
    }

    // The slide is intended to remain grounded.
    FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);

    if (!CurrentFloor.IsWalkableFloor())
    {
        SetMovementMode(MOVE_Falling);
        return;
    }

    SlideTimeRemaining -= DeltaTime;

    if (SlideTimeRemaining <= 0.f || Velocity.Size2D() < MinimumSlideSpeed)
    {
        StopSlide();
        return;
    }

    // Apply limited directional steering while preserving
    // most of the momentum accumulated at slide start.
    FVector Steering = Acceleration.GetClampedToMaxSize(SlideSteeringAcceleration);

    Steering.Z = 0.f;

    Velocity += Steering * DeltaTime;

    // Simple slide drag. This avoids normal walking braking
    // from rapidly eliminating the initial slide momentum.
    const float DragFactor =
        FMath::Max(0.f, 1.f - SlideFriction * DeltaTime);

    Velocity.X *= DragFactor;
    Velocity.Y *= DragFactor;

    // Keep speed bounded.
    const float Speed2D = Velocity.Size2D();

    if (Speed2D > MaximumSlideSpeed)
    {
        const FVector Limited = Velocity.GetSafeNormal2D() * MaximumSlideSpeed;

        Velocity.X = Limited.X;
        Velocity.Y = Limited.Y;
    }

    Velocity.Z = 0.f;

    const float CurrentSpeed = Velocity.Size2D();

    const float SpeedLoss = FMath::Min(
        CurrentSpeed,
        SlideBrakingDeceleration * DeltaTime);

    if (CurrentSpeed > KINDA_SMALL_NUMBER)
    {
        Velocity -= Velocity.GetSafeNormal2D() * SpeedLoss;
    }

    const FVector Delta = Velocity * DeltaTime;
    const FQuat Rotation = UpdatedComponent->GetComponentQuat();

    FHitResult Hit;

    SafeMoveUpdatedComponent(
        Delta,
        Rotation,
        true,
        Hit);

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(
            Delta,
            1.f - Hit.Time,
            Hit.Normal,
            Hit,
            true);
    }

    // Check ground contact after collision resolution.
    FindFloor(
        UpdatedComponent->GetComponentLocation(),
        CurrentFloor,
        false);

    if (!CurrentFloor.IsWalkableFloor())
    {
        SetMovementMode(MOVE_Falling);
        return;
    }

    Velocity.Z = 0.f;

    if (SlideTimeRemaining <= 0.f ||
        Velocity.Size2D() < MinimumSlideSpeed)
    {
        StopSlide();
    }
}