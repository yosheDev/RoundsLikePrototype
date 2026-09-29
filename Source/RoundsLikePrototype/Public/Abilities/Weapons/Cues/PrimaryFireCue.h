// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "PrimaryFireCue.generated.h"

class UAnimMontage;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UPrimaryFireCue : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UPrimaryFireCue();

	// The Montage to play when this cue is executed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue|Animation")
	UAnimMontage* MontageToPlay;

	// Start section name.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue|Animation")
	FName StartSectionName;

	// Speed of the animation playback.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue|Animation")
	float PlayRate;

protected:
	/** Overridden from GAS to handle the core 'Execute' logic */
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;
};
