// Copyright Jacob Jones 2026


#include "UI/HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBar::NativeConstruct()
{
    Super::NativeConstruct();

    // Set default health values.
    UpdateHealthBar(10000.0f, 10000.0f);
}

void UHealthBar::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    if (MaxHealth <= 0.0f) return;

    // Calculate a percentage between 0.0f and 1.0f for the progress bar
    float PercentAlpha = CurrentHealth / MaxHealth;

    if (HealthBar)
    {
        HealthBar->SetPercent(PercentAlpha);
    }
}