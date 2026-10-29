// Copyright Jacob Jones 2026

#include "Components/FPSCharacterMovementComponent.h"
#include "FPSCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

namespace SlideMove
{
    constexpr uint8 SlideFlag = FSavedMove_Character::FLAG_Custom_0;
}

void UFPSCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UFPSCharacterMovementComponent, SlideDirection);
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

    bSavedSliding = false;
    SavedSlideDirection = FVector::ForwardVector;

    bSavedWantsToStartSlide = false;
    SavedSlideTimeRemaining = 0.0f;
    SavedSlideStartDirection = FVector::ForwardVector;
    SavedMovementMode = MOVE_Walking;
    SavedCustomMovementMode = 0;
}

uint8 FSavedMove_Custom::GetCompressedFlags() const
{
    return FSavedMove_Character::GetCompressedFlags();
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

    if (const UFPSCharacterMovementComponent* MoveComp = Cast<UFPSCharacterMovementComponent>(Character->GetCharacterMovement()))
    {
        bSavedSliding = MoveComp->IsSliding();
        SavedSlideDirection = MoveComp->SlideDirection;

        SavedSlideTimeRemaining = MoveComp->SlideTimeRemaining;
        bSavedWantsToStartSlide = MoveComp->bPendingSlideStartRequest;
        SavedSlideStartDirection = MoveComp->PendingSlideStartDirection;
        SavedMovementMode = MoveComp->MovementMode;
        SavedCustomMovementMode = MoveComp->CustomMovementMode;
    }
}

void FSavedMove_Custom::PrepMoveFor(ACharacter* Character)
{
    FSavedMove_Character::PrepMoveFor(Character);

    if (!Character)
    {
        return;
    }

    if (UFPSCharacterMovementComponent* MoveComp = Cast<UFPSCharacterMovementComponent>(Character->GetCharacterMovement()))
    {
        MoveComp->RestoreSlideState(
            bSavedSliding,
            SavedSlideDirection,
            SavedSlideTimeRemaining);

        MoveComp->bPendingSlideStartRequest = bSavedWantsToStartSlide;
        MoveComp->PendingSlideStartDirection = SavedSlideStartDirection;

        if (MoveComp->MovementMode != SavedMovementMode || (SavedMovementMode == MOVE_Custom && MoveComp->CustomMovementMode != SavedCustomMovementMode))
        {
            MoveComp->SetMovementMode(SavedMovementMode, SavedCustomMovementMode);
        }
    }
}

bool FSavedMove_Custom::CanCombineWith(
    const FSavedMovePtr& NewMove,
    ACharacter* Character,
    float MaxDelta) const
{
    const FSavedMove_Custom* Other = static_cast<const FSavedMove_Custom*>(NewMove.Get());

    if (!Other)
    {
        return false;
    }

    if (bSavedSliding != Other->bSavedSliding ||
        !SavedSlideDirection.Equals(Other->SavedSlideDirection, 0.01f) ||
        bSavedWantsToStartSlide != Other->bSavedWantsToStartSlide ||
        bSavedWantsToStartSlide &&
        !SavedSlideStartDirection.Equals(Other->SavedSlideStartDirection, 0.01f) ||
        SavedMovementMode != Other->SavedMovementMode ||
        SavedCustomMovementMode != Other->SavedCustomMovementMode)
    {
        return false;
    }

    return FSavedMove_Character::CanCombineWith(NewMove, Character, MaxDelta);
}
#pragma endregion

#pragma region Network Move Data
void FSlideNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
    Super::ClientFillNetworkMoveData(ClientMove, MoveType);

    const FSavedMove_Custom& SlideMove = static_cast<const FSavedMove_Custom&>(ClientMove);

    bWantsToStartSlide = SlideMove.bSavedWantsToStartSlide;
    SlideStartDirection = SlideMove.SavedSlideStartDirection;
}

bool FSlideNetworkMoveData::Serialize(
    UCharacterMovementComponent& CharacterMovement,
    FArchive& Ar,
    UPackageMap* PackageMap,
    ENetworkMoveType MoveType)
{
    if (!Super::Serialize(
        CharacterMovement,
        Ar,
        PackageMap,
        MoveType))
    {
        return false;
    }

    Ar << bWantsToStartSlide;
    Ar << SlideStartDirection;

    return !Ar.IsError();
}
#pragma endregion

UFPSCharacterMovementComponent::UFPSCharacterMovementComponent()
{
    bOrientRotationToMovement = true;

    SetNetworkMoveDataContainer(SlideNetworkMoveDataContainer);
}

void UFPSCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
    Super::UpdateFromCompressedFlags(Flags);

    const FCharacterNetworkMoveData* BaseMoveData = GetCurrentNetworkMoveData();

    if (!BaseMoveData)
    {
        return;
    }

    const FSlideNetworkMoveData* SlideMoveData = static_cast<const FSlideNetworkMoveData*>(BaseMoveData);

    if (!SlideMoveData->bWantsToStartSlide)
    {
        return;
    }

    // The request is now consumed by the shared movement-state hook.
    bPendingSlideStartRequest = true;
    PendingSlideStartDirection = FVector(SlideMoveData->SlideStartDirection).GetSafeNormal2D();
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

    UE_LOG(LogTemp, Warning,
        TEXT("SlideModeChange: Role=%d Previous=%d/%d Current=%d/%d"),
        CharacterOwner ? (int32)CharacterOwner->GetLocalRole() : -1,
        (int32)PreviousMovementMode,
        (int32)PreviousCustomMode,
        (int32)MovementMode,
        (int32)CustomMovementMode);

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

void UFPSCharacterMovementComponent::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
    if (MovementMode != NewMovementMode || (NewMovementMode == MOVE_Custom && CustomMovementMode != NewCustomMode))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SlideNet: Role=%d Local=%d %d/%d -> %d/%d"),
            CharacterOwner ? (int32)CharacterOwner->GetLocalRole() : -1,
            CharacterOwner ? CharacterOwner->IsLocallyControlled() : false,
            (int32)MovementMode,
            (int32)CustomMovementMode,
            (int32)NewMovementMode,
            (int32)NewCustomMode);
    }

    Super::SetMovementMode(NewMovementMode, NewCustomMode);
}

void UFPSCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
    Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

    if (!bPendingSlideStartRequest)
    {
        return;
    }

    const FVector Direction = PendingSlideStartDirection;

    // Consume the request exactly once.
    bPendingSlideStartRequest = false;

    if (!IsSliding())
    {
        TryStartSlide(Direction);
    }
}

void UFPSCharacterMovementComponent::SmoothClientPosition(float DeltaSeconds)
{
    if (HasValidData() &&
        CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy &&
        IsSliding() &&
        NetworkSmoothingMode == ENetworkSmoothingMode::Exponential)
    {
        if (FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character())
        {
            // Preserve interpolation, but reduce the time the mesh
            // spends correcting its position during a slide.
            const float OriginalSmoothNetUpdateTime = ClientData->SmoothNetUpdateTime;

            ClientData->SmoothNetUpdateTime = FMath::Min(OriginalSmoothNetUpdateTime, SlideMeshSmoothingTime);

            Super::SmoothClientPosition(DeltaSeconds);

            ClientData->SmoothNetUpdateTime = OriginalSmoothNetUpdateTime;

            return;
        }
    }

    Super::SmoothClientPosition(DeltaSeconds);

    Super::SmoothClientPosition(DeltaSeconds);
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



#pragma region Sliding
void UFPSCharacterMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
    if (DeltaTime < MIN_TICK_TIME || !UpdatedComponent || !CharacterOwner)
    {
        return;
    }

    if (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
    {
        // Do not run a second local slide simulation for simulated proxies.
        return;
    }


    // The slide is intended to remain grounded.
    FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);

    if (!CurrentFloor.IsWalkableFloor())
    {
        Velocity = Velocity.GetClampedToMaxSize2D(MaxWalkSpeed);
        SetMovementMode(MOVE_Falling);
        return;
    }

    const bool bHasAuthority = CharacterOwner && CharacterOwner->HasAuthority();

    SlideTimeRemaining -= DeltaTime;

    if (SlideTimeRemaining <= 0.0f)
    {
        StopSlide();
        return;
    }


    // Linear deceleration from SlideSpeed to 0.5, make this better later.
    const float RemainingFraction = FMath::Clamp(
        SlideTimeRemaining / SlideDuration,
        0.5f,
        1.0f);

    const float CurrentSpeed = FMath::Max(SlideSpeed * RemainingFraction, MaxWalkSpeed);

    Velocity = SlideDirection * CurrentSpeed;

    /*const float SpeedLoss = FMath::Min(CurrentSpeed, SlideBrakingDeceleration * DeltaTime);

    if (CurrentSpeed > KINDA_SMALL_NUMBER)
    {
        Velocity -= Velocity.GetSafeNormal2D() * SpeedLoss;
    }*/

    // Preserve the slide speed, but align its direction to the slope.
    const FVector FloorNormal = CurrentFloor.HitResult.ImpactNormal;
    const float Speed2D = Velocity.Size2D();

    FVector GroundVelocity =
        FVector::VectorPlaneProject(Velocity, FloorNormal);

    if (!GroundVelocity.IsNearlyZero())
    {
        GroundVelocity =
            GroundVelocity.GetSafeNormal() * Speed2D;
    }

    Velocity = GroundVelocity;

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
        Velocity = Velocity.GetClampedToMaxSize2D(MaxWalkSpeed);
        SetMovementMode(MOVE_Falling);
        return;
    }

    /* UE_LOG(LogTemp, Warning,
         TEXT("SlideNet: Role=%d Mode=%d CustomMode=%d "
             "Location=%s Velocity=%s Time=%.3f"),
         CharacterOwner ? (int32)CharacterOwner->GetLocalRole() : -1,
         (int32)MovementMode,
         (int32)CustomMovementMode,
         *UpdatedComponent->GetComponentLocation().ToCompactString(),
         *Velocity.ToCompactString(),
         SlideTimeRemaining);*/
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
        && !IsSliding();
}

void UFPSCharacterMovementComponent::RequestSlide(const FVector& Direction)
{
    if (bPendingSlideStartRequest || IsSliding())
    {
        return;
    }

    const FVector NormalizedDirection = Direction.GetSafeNormal2D();

    if (NormalizedDirection.IsNearlyZero())
    {
        return;
    }

    bPendingSlideStartRequest = true;
    PendingSlideStartDirection = NormalizedDirection;
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

    Velocity.X = NewDirection.X * SlideSpeed;
    Velocity.Y = NewDirection.Y * SlideSpeed;
    Velocity.Z = 0.f;

    SlideDirection = NewDirection;
    SlideTimeRemaining = SlideDuration;
    bSlideRequested = true;

    SetMovementMode(MOVE_Custom, SlideCustomMode);

    if (CharacterOwner)
    {
        //CharacterOwner->Crouch();
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
        //CharacterOwner->UnCrouch();
    }

    UE_LOG(LogTemp, Warning,
        TEXT("SlideNet: StopSlide: Role=%d Mode=%d CustomMode=%d TimeRemaining=%.3f"),
        CharacterOwner ? (int32)CharacterOwner->GetLocalRole() : -1,
        (int32)MovementMode,
        (int32)CustomMovementMode,
        SlideTimeRemaining);
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
        SetMovementMode(MOVE_Walking);
    }
}

#pragma endregion