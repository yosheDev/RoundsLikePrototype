// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LoadingProcessInterface.h"
#include "MatchInstanceSubsystem.generated.h"

class AFPSPlayerState;

UENUM(BlueprintType)
enum class EMatchLoadingScreenType : uint8
{
	None,
	ArenaTravel,
	Connecting,
	ReturningToMenu
};

UENUM(BlueprintType)
enum class EMatchLoadingScreenPhase : uint8
{
	None,
	Intro,
	Loading,
	Outro
};

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
	WaitingForPlayers,
	AbilityDraft,
	RoundStarting,
	InRound,
	RoundEnd,
	MatchEnd
};

USTRUCT(BlueprintType)
struct FMatchData
{
	GENERATED_BODY()

	UPROPERTY()
	EMatchPhase MatchPhase = EMatchPhase::RoundStarting; // When this is "Waiting For Players" Server cannot move but client can. No proper functionality there yet.

	UPROPERTY()
	uint8 RoundNumber = 1;

	UPROPERTY()
	TObjectPtr<AFPSPlayerState> PlayerOne = nullptr;

	UPROPERTY()
	TObjectPtr<AFPSPlayerState> PlayerTwo = nullptr;

	UPROPERTY()
	int32 PlayerOneId = INDEX_NONE;

	UPROPERTY()
	int32 PlayerTwoId = INDEX_NONE;

	// Number of rounds player one has won.
	UPROPERTY()
	uint8 PlayerOneWins = 0;

	// Number of rounds player two has won.
	UPROPERTY()
	uint8 PlayerTwoWins = 0;

	// Number of wins player one has in ONLY the current round.
	UPROPERTY()
	uint8 PlayerOneRoundWins = 0;

	// Number of wins player two has in ONLY the current round.
	UPROPERTY()
	uint8 PlayerTwoRoundWins = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnLoadingScreenStateChanged,
	EMatchLoadingScreenType, LoadingType,
	EMatchLoadingScreenPhase, LoadingPhase
);

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UMatchInstanceSubsystem : public UGameInstanceSubsystem, public ILoadingProcessInterface
{
	GENERATED_BODY()
	
	public:
		virtual void Initialize(FSubsystemCollectionBase& Collection) override;
		virtual void Deinitialize() override;

#pragma region Loading Screen

		// Loading Screen Interface
		virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;

		// Loading Screen Control
		void SetLoadingScreen(EMatchLoadingScreenType NewType, EMatchLoadingScreenPhase NewPhase);

		UFUNCTION(BlueprintPure, Category = "Loading Screen")
		EMatchLoadingScreenType GetLoadingScreenType() const
		{
			return LoadingScreenType;
		}

		UFUNCTION(BlueprintPure, Category = "Loading Screen")
		EMatchLoadingScreenPhase GetLoadingScreenPhase() const
		{
			return LoadingScreenPhase;
		}

		UPROPERTY(BlueprintAssignable, Category = "Loading Screen")
		FOnLoadingScreenStateChanged OnLoadingScreenStateChanged;

private:

	UPROPERTY()
	EMatchLoadingScreenType LoadingScreenType = EMatchLoadingScreenType::None;

	UPROPERTY()
	EMatchLoadingScreenPhase LoadingScreenPhase = EMatchLoadingScreenPhase::None;

#pragma endregion

	#pragma region Match Data
	public:
		
		UPROPERTY()
		FMatchData MatchData;

		FMatchData& GetMatchData()
		{
			return MatchData;
		}

		void SetMatchData(FMatchData NewData)
		{
			MatchData = NewData;
		}

		void InitializeMatchData()
		{
			FMatchData NewMatchData;
			MatchData = NewMatchData;
		}
	#pragma endregion
};
