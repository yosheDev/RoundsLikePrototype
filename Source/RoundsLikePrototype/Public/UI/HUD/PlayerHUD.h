// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

// Forward declarations
class UTextBlock;
class UProgressBar;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Update visual display of the health bar.
	void UpdateHealthBar(float CurrentHealth, float MaxHealth);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Bar|Colors")
	FLinearColor HealthColorLow = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Bar|Colors")
	FLinearColor HealthColorMid = FLinearColor::Yellow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Bar|Colors")
	FLinearColor HealthColorHigh = FLinearColor::Green;

protected:

	virtual void NativeConstruct() override;

	// IMPORTANT: "meta = (BindWidget)" is what allows the c++ to connect to actual widget in designer. MUST match name.

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;
};
