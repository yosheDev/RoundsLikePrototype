// Copyrighted Jacob Jones 2026


#include "FPSPlayerState.h"
#include "Components/FPSAbilitySystemComponent.h"

AFPSPlayerState::AFPSPlayerState()
{
	FPSAbilitySystemComponent = CreateDefaultSubobject<UFPSAbilitySystemComponent>(TEXT("AbilitySystem"));

	VitalityAttributeSet = CreateDefaultSubobject<UVitalityAttributeSet>(TEXT("VitalityAttributeSet"));
	MovementAttributeSet = CreateDefaultSubobject<UMovementAttributeSet>(TEXT("MovementAttributeSet"));
	GunplayAttributeSet = CreateDefaultSubobject<UGunplayAttributeSet>(TEXT("GunplayAttributeSet"));

	FPSAbilitySystemComponent->SetIsReplicated(true);
	FPSAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AFPSPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AFPSPlayerState::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

UAbilitySystemComponent* AFPSPlayerState::GetAbilitySystemComponent() const
{
	return FPSAbilitySystemComponent;
}
