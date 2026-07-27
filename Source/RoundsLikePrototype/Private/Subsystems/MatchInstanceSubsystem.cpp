// Copyright Jacob Jones 2026


#include "Subsystems/MatchInstanceSubsystem.h"

void UMatchInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Log, TEXT("MatchInstanceSubsystem Initialized"));
}

void UMatchInstanceSubsystem::Deinitialize()
{
    Super::Deinitialize();
}