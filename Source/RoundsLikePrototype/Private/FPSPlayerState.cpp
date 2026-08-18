// Copyrighted Jacob Jones 2026


#include "FPSPlayerState.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "Engine/DataTable.h"
#include "GameplayEffectTypes.h"
#include "Subsystems/MatchInstanceSubsystem.h"
#include "Abilities/AbilityDefinition.h"
#include "Abilities/AbilityDefinitionHelper.h"

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

void AFPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerState, AccruedAbilities);
}

void AFPSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AFPSPlayerState* NewPlayerState = Cast<AFPSPlayerState>(PlayerState);
	NewPlayerState->SavedVitalityAttributesMap.Empty();

	#pragma region VitalityAttributeSet
	for (TFieldIterator<FProperty> PropIt(VitalityAttributeSet->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
		{
			if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
			{
				const FGameplayAttributeData* AttributeData =
					StructProp->ContainerPtrToValuePtr<FGameplayAttributeData>(VitalityAttributeSet);

				if (AttributeData)
				{
					const FString AttributeName = Property->GetName();
					const float CurrentValue = AttributeData->GetCurrentValue();

					NewPlayerState->SavedVitalityAttributesMap.Add(
						AttributeName,
						CurrentValue);
				}
			}
		}
	}
	#pragma endregion
	
	#pragma region MovementAttributeSet
	NewPlayerState->SavedMovementAttributesMap.Empty();

	for (TFieldIterator<FProperty> PropIt(MovementAttributeSet->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
		{
			if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
			{
				const FGameplayAttributeData* AttributeData =
					StructProp->ContainerPtrToValuePtr<FGameplayAttributeData>(MovementAttributeSet);

				if (AttributeData)
				{
					const FString AttributeName = Property->GetName();
					const float CurrentValue = AttributeData->GetCurrentValue();

					NewPlayerState->SavedMovementAttributesMap.Add(
						AttributeName,
						CurrentValue);
				}
			}
		}
	}
	#pragma endregion
	
	#pragma region GunplayAttributeSet
	NewPlayerState->SavedGunplayAttributesMap.Empty();

	for (TFieldIterator<FProperty> PropIt(GunplayAttributeSet->GetClass()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
		{
			if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
			{
				const FGameplayAttributeData* AttributeData =
					StructProp->ContainerPtrToValuePtr<FGameplayAttributeData>(GunplayAttributeSet);

				if (AttributeData)
				{
					const FString AttributeName = Property->GetName();
					const float CurrentValue = AttributeData->GetCurrentValue();

					NewPlayerState->SavedGunplayAttributesMap.Add(
						AttributeName,
						CurrentValue);
				}
			}
		}
	}
	#pragma endregion

	NewPlayerState->AccruedAbilities = AccruedAbilities;

	UE_LOG(LogTemp, Warning,
		TEXT("=== TRAVEL: CopyProperties === MaxSpeed Saved: %f"),
		SavedMovementAttributesMap.FindRef(TEXT("MaxSpeed")));
}

void AFPSPlayerState::RestoreAttributesAfterTravel()
{
	if (!(!VitalityAttributeSet || SavedVitalityAttributesMap.Num() == 0))
	{
		/** Loop through the variables on the new VitalityAttributeSet */
		for (TFieldIterator<FProperty> PropIt(VitalityAttributeSet->GetClass()); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;

			if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
			{
				if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
				{
					FString AttributeName = Property->GetName();

					/** Look up saved value for a specific attribute name */
					if (SavedVitalityAttributesMap.Contains(AttributeName))
					{
						FGameplayAttributeData* AttributeData = StructProp->ContainerPtrToValuePtr<FGameplayAttributeData>(VitalityAttributeSet);
						if (AttributeData)
						{
							/** Overwrite the default initialization value with the saved value */
							float SavedValue = SavedVitalityAttributesMap[AttributeName];

							AttributeData->SetBaseValue(SavedValue);
							AttributeData->SetCurrentValue(SavedValue);
						}
					}
				}
			}
		}
	}

	if (!(!MovementAttributeSet || SavedMovementAttributesMap.Num() == 0))
	{
		/** Loop through the variables on the new MovementAttributeSet */
		for (TFieldIterator<FProperty> PropIt(MovementAttributeSet->GetClass()); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;

			if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
			{
				if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
				{
					FString AttributeName = Property->GetName();

					/** Look up saved value for a specific attribute name */
					if (SavedMovementAttributesMap.Contains(AttributeName))
					{
						FGameplayAttributeData* AttributeData = StructProp->ContainerPtrToValuePtr<FGameplayAttributeData>(MovementAttributeSet);
						if (AttributeData)
						{
							/** Overwrite the default initialization value with the saved value */
							float SavedValue = SavedMovementAttributesMap[AttributeName];

							AttributeData->SetBaseValue(SavedValue);
							AttributeData->SetCurrentValue(SavedValue);
						}
					}
				}
			}
		}
	}
	
	if (!(!GunplayAttributeSet || SavedGunplayAttributesMap.Num() == 0))
	{
		/** Loop through the variables on the new GunplayAttributeSet */
		for (TFieldIterator<FProperty> PropIt(GunplayAttributeSet->GetClass()); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;

			if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
			{
				if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
				{
					FString AttributeName = Property->GetName();

					/** Look up saved value for a specific attribute name */
					if (SavedGunplayAttributesMap.Contains(AttributeName))
					{
						FGameplayAttributeData* AttributeData = StructProp->ContainerPtrToValuePtr<FGameplayAttributeData>(GunplayAttributeSet);
						if (AttributeData)
						{
							/** Overwrite the default initialization value with the saved value */
							float SavedValue = SavedGunplayAttributesMap[AttributeName];

							AttributeData->SetBaseValue(SavedValue);
							AttributeData->SetCurrentValue(SavedValue);
						}
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning,
		TEXT("=== TRAVEL: RestoreAttributesAfterTravel === MaxSpeed Saved: %f"),
		SavedMovementAttributesMap.FindRef(TEXT("MaxSpeed")));

	SavedVitalityAttributesMap.Empty();
	SavedMovementAttributesMap.Empty();
	SavedGunplayAttributesMap.Empty();

	ReapplyAbilitiesAfterTravel();
}

void AFPSPlayerState::ReapplyAbilitiesAfterTravel()
{
	if (!FPSAbilitySystemComponent || AccruedAbilities.Num() == 0)
	{
		return;
	}
	
	// Force a scan of all AbilityDefinitions. Done since this is called immediately after travel, when AssetManager is not guarenteed to have scanned yet.
	AbilityDefinitions::Scan();

	for (const FGameplayTag& AbilityTag : AccruedAbilities)
	{
		AbilityDefinitions::Find(AbilityTag, [this, AbilityTag](UAbilityDefinition* Definition)
		{
			if (!Definition)
			{
				UE_LOG(LogTemp, Warning, TEXT("ReapplyAbilitiesAfterTravel: Failed to find UAbilityDefinition for tag %s! Skipping."), *AbilityTag.ToString());
				return;
			}

			// Grant all abilities provided by the Ability Definition.
			for (const TSubclassOf<UGameplayAbility>& AbilityClass : Definition->GASAbilities)
			{
				if (AbilityClass)
				{
					FGameplayAbilitySpec Spec(AbilityClass, 1, -1, this);
					FPSAbilitySystemComponent->GiveAbility(Spec);
				}
			}

			// Apply all effects provided by the Ability Definition.
			for (const TSubclassOf<UGameplayEffect>& EffectClass : Definition->GASEffects)
			{
				UE_LOG(LogTemp, Log, TEXT("TRAVEL - Reapplying effect: [%s]"), *GetNameSafe(EffectClass));

				if (EffectClass)
				{
					FGameplayEffectContextHandle EffectContext = FPSAbilitySystemComponent->MakeEffectContext();

					EffectContext.AddSourceObject(this);

					FGameplayEffectSpecHandle NewSpecHandle = FPSAbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1, EffectContext);

					if (NewSpecHandle.IsValid())
					{
						FPSAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewSpecHandle.Data.Get());
					}
				}
			}
		});
	}
}

void AFPSPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AFPSPlayerState::PostInitializeComponents()
{
	// This will only need to be done when FIRST spawning in? Not needed when doing the copy -> restore process for attribute sets.
	Super::PostInitializeComponents();

	// Upon loading in ONLY ON THE FIRST ROUND, initialize all attribute sets to be the defaults.
	if (GetWorld()->GetGameInstance()->GetSubsystem<UMatchInstanceSubsystem>()->MatchData.RoundNumber <= 1)
	{
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

void AFPSPlayerState::Server_AddAccruedAbility_Implementation(FGameplayTag AbilityTag)
{
	AccruedAbilities.AddUnique(AbilityTag);
}
