// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

// Forward declarations
class UTextBlock;
class UProgressBar;
class UAmmoRadialSlider;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Update visual display of the health bar.
	UFUNCTION(Category = "Health Bar")
	void UpdateHealthBar(float CurrentHealth, float MaxHealth);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Bar|Colors")
	FLinearColor HealthColorLow = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Bar|Colors")
	FLinearColor HealthColorMid = FLinearColor::Yellow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Bar|Colors")
	FLinearColor HealthColorHigh = FLinearColor::Green;

	UFUNCTION(Category = "Ammo UI")
	void UpdateAmmoSlider(int32 CurrentAmmo, int32 MaxAmmo, float AmmoRegenRemainingTime, float AmmoRegenDuration);

protected:

	virtual void NativeConstruct() override;

	// IMPORTANT: "meta = (BindWidget)" is what allows the c++ to connect to actual widget in designer. MUST match name.

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UAmmoRadialSlider* AmmoRadialSlider;
};
