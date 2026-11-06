// Copyright Jacob Jones 2026


#include "Subsystems/MatchInstanceSubsystem.h"
#include "LoadingScreenManager.h"

void UMatchInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Ensure the CommonLoadingScreen manager is initialized first.
    Collection.InitializeDependency<ULoadingScreenManager>();

    if (ULoadingScreenManager* LoadingScreenManager = GetGameInstance()->GetSubsystem<ULoadingScreenManager>())
    {
        LoadingScreenManager->RegisterLoadingProcessor(this);
    }

    UE_LOG(LogTemp, Log, TEXT("MatchInstanceSubsystem Initialized"));
}

void UMatchInstanceSubsystem::Deinitialize()
{
    if (ULoadingScreenManager* LoadingScreenManager = GetGameInstance()->GetSubsystem<ULoadingScreenManager>())
    {
        LoadingScreenManager->UnregisterLoadingProcessor(this);
    }

    Super::Deinitialize();
}

bool UMatchInstanceSubsystem::ShouldShowLoadingScreen(FString& OutReason) const
{
    if (LoadingScreenType == EMatchLoadingScreenType::None)
    {
        return false;
    }

    switch (LoadingScreenType)
    {
    case EMatchLoadingScreenType::ArenaTravel:
        OutReason = TEXT("Loading next arena");
        return true;

    case EMatchLoadingScreenType::Connecting:
        OutReason = TEXT("Connecting to match");
        return true;

    case EMatchLoadingScreenType::ReturningToMenu:
        OutReason = TEXT("Returning to menu");
        return true;

    default:
        return false;
    }
}

void UMatchInstanceSubsystem::SetLoadingScreen(EMatchLoadingScreenType NewType, EMatchLoadingScreenPhase NewPhase)
{
    LoadingScreenType = NewType;
    LoadingScreenPhase = NewPhase;

    OnLoadingScreenStateChanged.Broadcast(LoadingScreenType, LoadingScreenPhase);
    UE_LOG(LogTemp, Log, TEXT("TransitionLog: MatchInstanceSubsystem() has broadcast the new screen type and screen phase."));
}