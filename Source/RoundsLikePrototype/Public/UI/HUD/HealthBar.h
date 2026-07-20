// Copyright Jacob Jones 2026

#pragma once

// Preprocessor directives
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

// Forward declarations
class UProgressBar;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // Update visual display of the health bar.
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);

protected:

    virtual void NativeConstruct() override;

    // IMPORTANT: "meta = (BindWidget)" is what allows the c++ to connect to actual widget in designer. MUST match name.

    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;
};
