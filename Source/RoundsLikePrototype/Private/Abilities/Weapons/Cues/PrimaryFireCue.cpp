// Copyright Jacob Jones 2026


#include "Abilities/Weapons/Cues/PrimaryFireCue.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

UPrimaryFireCue::UPrimaryFireCue()
{
	// Set safe defaults
	MontageToPlay = nullptr;
	StartSectionName = NAME_None;
	PlayRate = 1.0f;
}

bool UPrimaryFireCue::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	Super::OnExecute_Implementation(MyTarget, Parameters);
	if (!MontageToPlay) { return false; }

	// Unpack the Weapon Actor passed via network payload in SourceObject
	AActor* WeaponActor = const_cast<AActor*>(Cast<AActor>(Parameters.SourceObject.Get()));
	
	// Fallback: If no explicit source object was provided, check if MyTarget is a weapon actor
	if (!WeaponActor) { WeaponActor = MyTarget; }
	if (!WeaponActor) { return false; }

	UE_LOG(LogTemp, Log, TEXT("Primary Fire Cue: Weapon Actor: [%s]"), *GetNameSafe(WeaponActor));

	USkeletalMeshComponent* MeshComp = WeaponActor->FindComponentByClass<USkeletalMeshComponent>();
	UE_LOG(LogTemp, Log, TEXT("Primary Fire Cue: Mesh Component: [%s]"), *GetNameSafe(MeshComp));
	
	if (MeshComp)
	{
		if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
		{
			const float MontageLength = AnimInstance->Montage_Play(MontageToPlay, PlayRate);

			if (MontageLength <= 0.0f)
			{
				UE_LOG(LogTemp, Error, TEXT("Primary Fire Cue: Montage_Play FAILED."));
				return false;
			}

			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, MontageToPlay);
			}

			return true;	
		}
	}

	return false;
}