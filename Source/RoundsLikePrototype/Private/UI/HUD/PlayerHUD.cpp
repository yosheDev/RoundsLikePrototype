// Copyright Jacob Jones 2026


#include "UI/HUD/PlayerHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Math/Color.h"

void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();

    // Set default health values.
    UpdateHealthBar(100.0f, 100.0f);
}

void UPlayerHUD::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    if (MaxHealth <= 0.0f) return;

    // Calculate a percentage between 0.0f and 1.0f for the progress bar
    float PercentAlpha = CurrentHealth / MaxHealth;

    if (HealthBar)
    {
        HealthBar->SetPercent(PercentAlpha);

        #pragma region Health Bar Color
        FLinearColor BarColor;

        if (PercentAlpha <= 0.2f)
        {
            BarColor = HealthColorLow;
        }
        else if (PercentAlpha <= 0.50f)
        {
            float Alpha = (PercentAlpha - 0.2f) / 0.3f;
            BarColor = FLinearColor::LerpUsingHSV(HealthColorLow, HealthColorMid, Alpha);
        }
        else if (PercentAlpha <= 0.8f)
        {
            float Alpha = (PercentAlpha - 0.50f) / 0.3f;
            BarColor = FLinearColor::LerpUsingHSV(HealthColorMid, HealthColorHigh, Alpha);
        }
        else
        {
            BarColor = HealthColorHigh;
        }

        HealthBar->SetFillColorAndOpacity(BarColor);
        #pragma endregion
    }

    if (HealthText)
    {
        FNumberFormattingOptions Options;
        Options.MinimumFractionalDigits = 0;
        Options.MaximumFractionalDigits = 0;

        FText FormattedText = FText::AsNumber(CurrentHealth, &Options);
        HealthText->SetText(FormattedText);
    }
}