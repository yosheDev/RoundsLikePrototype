// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MatchInstanceSubsystem.generated.h"

class AFPSPlayerState;

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
	uint8 RoundNumber = 0;

	UPROPERTY()
	TObjectPtr<AFPSPlayerState> PlayerOne = nullptr;

	UPROPERTY()
	TObjectPtr<AFPSPlayerState> PlayerTwo = nullptr;

	UPROPERTY()
	uint8 PlayerOneWins = 0;

	UPROPERTY()
	uint8 PlayerTwoWins = 0;
};

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UMatchInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	public:
		virtual void Initialize(FSubsystemCollectionBase& Collection) override;
		virtual void Deinitialize() override;

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
};
