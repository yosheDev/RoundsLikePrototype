// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/MatchInstanceSubsystem.h"
#include "UI/LoadingScreen/TransitionSticker.h"
#include "TransitionLoadingScreen.generated.h"

class UCanvasPanel;
class UTextBlock;


UCLASS()
class ROUNDSLIKEPROTOTYPE_API UTransitionLoadingScreen : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMatchLoadingScreenType CurrentLoadingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMatchLoadingScreenPhase CurrentLoadingPhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasAcknowledgedCurrentPhase;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MainCanvas;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Loading Screen")
	void StartIntro();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Loading Screen")
	void StartOutro();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Loading Screen")
	void OnIntroFinished();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Loading Screen")
	void OnOutroFinished();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loading Screen | Stickers")
	TSubclassOf<UTransitionSticker> StickerWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loading Screen | Stickers")
	float StickerSpawnInterval = 0.025f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loading Screen | Stickers")
	int32 GridColumns = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loading Screen | Stickers")
	int32 GridRows = 6;

private:

	#pragma region Start/Setup Functions For Intro/Outros Of Differing Load Types
	// These can be filtered and called from StartIntro().
	// Intro dispatch
	void StartArenaTravelIntro();
	void StartConnectingIntro();
	void StartReturningToMenuIntro();

	// Outro dispatch
	void StartArenaTravelOutro();
	void StartConnectingOutro();
	void StartReturningToMenuOutro();

	// Shared sticker functionality
	void SetupStickerIntro();
	void SetupStickerOutro();
	#pragma endregion

	// Required to delay intro 1 tick so geometry can setup and canvas size is known.
	FTimerHandle IntroSetupTimerHandle;
	void SetupIntro();

	struct FStickerPlacement
	{
		FVector2D Position;
		FVector2D Size;
		float Rotation = 0.0f;
	};

	UPROPERTY()
	TArray<TObjectPtr<UTransitionSticker>> SpawnedStickers;

	TArray<FStickerPlacement> StickerPlacements;

	FTimerHandle StickerTimerHandle;

	int32 NextStickerIndex = 0;

	void SpawnNextSticker();
	void RemoveNextSticker();
};
