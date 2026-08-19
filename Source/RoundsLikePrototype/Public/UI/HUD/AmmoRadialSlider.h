// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/RadialSlider.h"
#include "AmmoRadialSlider.generated.h"

class URadialSlider;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UAmmoRadialSlider : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Category = "AmmoRadialSlider")
	void UpdateAmmoSlider(int32 CurrentAmmo, int32 MaxAmmo, float AmmoRegenRemainingTime, float AmmoRegenDuration);

protected:
	virtual void NativeConstruct() override;

public:

	UPROPERTY(meta = (BindWidget))
	URadialSlider* CurrentAmmoSlider;

	UPROPERTY(meta = (BindWidget))
	URadialSlider* AmmoRegenSlider;

	UPROPERTY(meta = (BindWidget))
	URadialSlider* BackgroundSlider;
};
