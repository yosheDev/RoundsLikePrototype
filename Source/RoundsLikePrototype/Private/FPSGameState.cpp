// Copyright Jacob Jones 2026


#include "FPSGameState.h"
#include "FPSHudController.h"
#include "Subsystems/MatchInstanceSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveFloat.h"
#include "HAL/PlatformTime.h"
#include "Components/MatchEconomyComponent.h"
#include "LoadingProcessInterface.h"

AFPSGameState::AFPSGameState()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    EconomyComponent = CreateDefaultSubobject<UMatchEconomyComponent>(TEXT("EconomyComponent"));
}

void AFPSGameState::BeginPlay()
{
    Super::BeginPlay();

    UpdateLocalLoadingScreen();
}

void AFPSGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Restore time dilation before this GameState is destroyed.
    if (bKillSlowdownActive)
    {
        UGameplayStatics::SetGlobalTimeDilation(this, OriginalTimeDilation);
    }

    bKillSlowdownActive = false;

    Super::EndPlay(EndPlayReason);
}

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFPSGameState, LoadingScreenType);
    DOREPLIFETIME(AFPSGameState, LoadingScreenPhase);
	DOREPLIFETIME(AFPSGameState, MatchPhase);
    DOREPLIFETIME(AFPSGameState, RoundNumber);
    DOREPLIFETIME(AFPSGameState, PlayerOneWins);
    DOREPLIFETIME(AFPSGameState, PlayerTwoWins);
    DOREPLIFETIME(AFPSGameState, PlayerOneRoundWins);
    DOREPLIFETIME(AFPSGameState, PlayerTwoRoundWins);
}

#pragma region Loading Process
void AFPSGameState::SetLoadingScreenState(EMatchLoadingScreenType NewType, EMatchLoadingScreenPhase NewPhase)
{
    if (!HasAuthority())
    {
        return;
    }

    LoadingScreenType = NewType;
    LoadingScreenPhase = NewPhase;

    UpdateLocalLoadingScreen();

    ForceNetUpdate();
}

void AFPSGameState::OnRep_LoadingScreenType()
{
    UpdateLocalLoadingScreen();
}

void AFPSGameState::OnRep_LoadingScreenPhase()
{
    UpdateLocalLoadingScreen();
}

void AFPSGameState::UpdateLocalLoadingScreen()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UMatchInstanceSubsystem* MatchSubsystem = GameInstance->GetSubsystem<UMatchInstanceSubsystem>())
        {
            MatchSubsystem->SetLoadingScreen(LoadingScreenType, LoadingScreenPhase);
        }
    }
}

bool AFPSGameState::ShouldShowLoadingScreen(FString& OutReason) const
{
    switch (LoadingScreenType)
    {
    case EMatchLoadingScreenType::ArenaTravel:
        OutReason = TEXT("Loading Arena");
        return true;

    case EMatchLoadingScreenType::Connecting:
        OutReason = TEXT("Connecting");
        return true;

    case EMatchLoadingScreenType::ReturningToMenu:
        OutReason = TEXT("Returning to Menu");
        return true;

    default:
        return false;
    }
}
#pragma endregion

void AFPSGameState::OnRep_MatchPhase()
{
    HandleMatchPhaseChanged();
}

// Tells local controllers that the match phase has changed. They respond accordingly.
void AFPSGameState::HandleMatchPhaseChanged()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It))
        {
            PC->Client_OnMatchPhaseChanged(MatchPhase);
            
            // Keep UI Selection eligibility server-authoratative.
            if (MatchPhase == EMatchPhase::AbilityDraft && GetCurrentLoserState() == PC->GetPlayerState<AFPSPlayerState>())
            {
                PC->Client_SetCanSelectUI(true);
            }
            else if (MatchPhase == EMatchPhase::AbilityDraft)
            {
                PC->Client_SetCanSelectUI(false);
            }
        }
    }
}

// Tells local controllers to display the match state.
void AFPSGameState::HandlePresentMatchState()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It))
        {
            UE_LOG(LogTemp, Log, TEXT("PresentationLog: FPSGameState: Presenting Match State on [%s]"), *PC->GetName());
            PC->Client_PresentMatchState(MakePlayerData(0), MakePlayerData(1), (CurrentLoserState == PlayerOne ? 0 : 1));
        }
    }
}

void AFPSGameState::OnRep_CurrentLoserState()
{

}

void AFPSGameState::OnRep_MatchWinnerState()
{

}

void AFPSGameState::UpdateMatchSubsystem()
{
    if (!HasAuthority())
    {
        return;
    }

    UMatchInstanceSubsystem* MatchSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMatchInstanceSubsystem>();

    FMatchData NewData;

    NewData.MatchPhase = MatchPhase;
    NewData.RoundNumber = RoundNumber;
    NewData.PlayerOne = PlayerOne;
    NewData.PlayerTwo = PlayerTwo;
    NewData.PlayerOneWins = PlayerOneWins;
    NewData.PlayerTwoWins = PlayerTwoWins;
    NewData.PlayerOneRoundWins = PlayerOneRoundWins;
    NewData.PlayerTwoRoundWins = PlayerTwoRoundWins;
    NewData.PlayerOneId = IsValid(PlayerOne) ? PlayerOne->GetPlayerId() : MatchSubsystem->MatchData.PlayerOneId;
    NewData.PlayerTwoId = IsValid(PlayerTwo) ? PlayerTwo->GetPlayerId() : MatchSubsystem->MatchData.PlayerTwoId;

    MatchSubsystem->SetMatchData(NewData);
    UE_LOG(LogTemp, Log, TEXT("MatchLog: MatchSubsystem is updated."));
}

void AFPSGameState::InitializeMatchData()
{
    UMatchInstanceSubsystem* MatchSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMatchInstanceSubsystem>();

    FMatchData NewData = MatchSubsystem->GetMatchData();

    MatchPhase = NewData.MatchPhase;
    RoundNumber = NewData.RoundNumber;
    PlayerOne = NewData.PlayerOne;
    PlayerTwo = NewData.PlayerTwo;
    PlayerOneWins = NewData.PlayerOneWins;
    PlayerTwoWins = NewData.PlayerTwoWins;
    PlayerOneRoundWins = NewData.PlayerOneRoundWins;
    PlayerTwoRoundWins = NewData.PlayerTwoRoundWins;

    UE_LOG(LogTemp, Log, TEXT("MatchLog: Initialized MatchData on GameState from Subsystem Data."));
}

FGamePlayerData AFPSGameState::MakePlayerData(uint8 PlayerID) const
{
    FGamePlayerData PlayerData;

    if (PlayerID == 0)
    {
        PlayerData.Username = FName("Player 1");
        PlayerData.Points = PlayerOneWins;
        PlayerData.RoundWins = PlayerOneRoundWins;
    }
    else if (PlayerID == 1)
    {
        PlayerData.Username = FName("Player 2");
        PlayerData.Points = PlayerTwoWins;
        PlayerData.RoundWins = PlayerTwoRoundWins;
    }

    return PlayerData;
}

AFPSPlayerState* AFPSGameState::GetCurrentLoserState()
{
    return CurrentLoserState;
}

#pragma region Kill Effects
void AFPSGameState::StartKillSlowdown()
{
    // Only the server should initiate the effect.
    if (!HasAuthority())
    {
        return;
    }

    Multicast_StartKillSlowdown();
}

void AFPSGameState::Multicast_StartKillSlowdown_Implementation()
{
    if (!KillSlowdownCurve)
    {
        return;
    }

    // Preserve the original value when starting a new effect.
    // Don't overwrite it if we're restarting an active effect.
    if (!bKillSlowdownActive)
    {
        OriginalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(this);
    }

    KillSlowdownStartTime = FPlatformTime::Seconds();

    bKillSlowdownActive = true;

    UpdateKillSlowdown();
}

void AFPSGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bKillSlowdownActive)
    {
        UpdateKillSlowdown();
    }
}

void AFPSGameState::UpdateKillSlowdown()
{
    if (!KillSlowdownCurve)
    {
        bKillSlowdownActive = false;
        return;
    }

    // Use real elapsed time, unaffected by global time dilation.
    const double CurrentTime = FPlatformTime::Seconds();

    const float ElapsedTime = static_cast<float>(CurrentTime - KillSlowdownStartTime);

    if (ElapsedTime >= KillSlowdownDuration)
    {
        StopKillSlowdown();
        return;
    }

    // Evaluate the curve.
    const float NewTimeDilation = KillSlowdownCurve->GetFloatValue(ElapsedTime);

    UGameplayStatics::SetGlobalTimeDilation(this, FMath::Max(NewTimeDilation, 0.01f));
}

void AFPSGameState::StopKillSlowdown()
{
    if (!bKillSlowdownActive)
    {
        return;
    }

    UGameplayStatics::SetGlobalTimeDilation(
        this,
        OriginalTimeDilation
    );

    bKillSlowdownActive = false;
}
#pragma endregion
