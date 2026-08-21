// Copyright Jacob Jones 2026


#include "UI/HUD/AmmoRadialSlider.h"
#include "Components/RadialSlider.h"

void UAmmoRadialSlider::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAmmoRadialSlider::UpdateAmmoSlider(int32 CurrentAmmo, int32 MaxAmmo, float AmmoRegenRemainingTime, float AmmoRegenDuration)
{
	float AmmoSectionLength = 1.0f / MaxAmmo;

	UE_LOG(LogTemp, Log, TEXT("CurrentAmmo: [%f]"), static_cast<float>(CurrentAmmo));
	UE_LOG(LogTemp, Log, TEXT("MaxAmmo: [%f]"), static_cast<float>(MaxAmmo));
	// This will be current ammo slider value + Ammo Section Length * Regen Time (0-1)
	// 
	// Update Current Ammo bar. CurrentAmmo / MaxAmmo.
	CurrentAmmoSlider->SetValue(static_cast<float>(CurrentAmmo) / static_cast<float>(MaxAmmo));
	UE_LOG(LogTemp, Log, TEXT("CurrentAmmoSlider: [%f]"), CurrentAmmoSlider->Value);
	UE_LOG(LogTemp, Log, TEXT("AmmoRegenRemainingTime: [%f]"), static_cast<float>(AmmoRegenRemainingTime));
	// This will be current ammo slider value + Ammo Section Length * Regen Time (0-1)
	AmmoRegenSlider->SetValue(CurrentAmmoSlider->Value + ( (1.0f - (AmmoRegenRemainingTime / AmmoRegenDuration)) * AmmoSectionLength));
}