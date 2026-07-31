// Copyrighted Jacob Jones 2026


#include "FPSPlayerState.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "Engine/DataTable.h"
#include "GameplayEffectTypes.h"

AFPSPlayerState::AFPSPlayerState()
{
	FPSAbilitySystemComponent = CreateDefaultSubobject<UFPSAbilitySystemComponent>(TEXT("AbilitySystem"));

	VitalityAttributeSet = CreateDefaultSubobject<UVitalityAttributeSet>(TEXT("VitalityAttributeSet"));
	MovementAttributeSet = CreateDefaultSubobject<UMovementAttributeSet>(TEXT("MovementAttributeSet"));
	GunplayAttributeSet = CreateDefaultSubobject<UGunplayAttributeSet>(TEXT("GunplayAttributeSet"));

	FPSAbilitySystemComponent->AddSpawnedAttribute(VitalityAttributeSet);
	FPSAbilitySystemComponent->AddSpawnedAttribute(MovementAttributeSet);
	FPSAbilitySystemComponent->AddSpawnedAttribute(GunplayAttributeSet);

	FPSAbilitySystemComponent->SetIsReplicated(true);
	FPSAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	UE_LOG(LogTemp, Warning,
		TEXT("MYTEST PlayerState ASC: %s"),
		*GetNameSafe(FPSAbilitySystemComponent));

	UE_LOG(LogTemp, Warning,
		TEXT("MYTEST PlayerState Gunplay Set: %s"),
		*GetNameSafe(GunplayAttributeSet));

	UE_LOG(LogTemp, Warning,
		TEXT("MYTEST PlayerState Gunplay Speed: %f"),
		GunplayAttributeSet->GetBulletSpeed());
}

void AFPSPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AFPSPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (VitalityAttributeSet && VitalityAttributeDataTable)
	{
		VitalityAttributeSet->InitFromMetaDataTable(VitalityAttributeDataTable);
	}
	if (MovementAttributeSet && MovementAttributeDataTable)
	{
		MovementAttributeSet->InitFromMetaDataTable(MovementAttributeDataTable);
	}
	if (GunplayAttributeSet && GunplayAttributeDataTable)
	{
		GunplayAttributeSet->InitFromMetaDataTable(GunplayAttributeDataTable);
	}

	UE_LOG(LogTemp, Warning,
		TEXT("MYTEST PlayerState BulletSpeed after init: %f"),
		GunplayAttributeSet->GetBulletSpeed());
}

void AFPSPlayerState::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

UAbilitySystemComponent* AFPSPlayerState::GetAbilitySystemComponent() const
{
	return FPSAbilitySystemComponent;
}
