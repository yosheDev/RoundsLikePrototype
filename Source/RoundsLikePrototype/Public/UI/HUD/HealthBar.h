// Copyright Jacob Jones 2026

#pragma once

// Preprocessor directives
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

// Forward declarations
class UProgressBar;

/*
 * NOTE: This health bar is specifically for world display above other players heads. HUD Health Bar is on PlayerHUD
 */

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // Update visual display of the health bar.
    UFUNCTION(BlueprintCallable)
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);

protected:

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;
};
