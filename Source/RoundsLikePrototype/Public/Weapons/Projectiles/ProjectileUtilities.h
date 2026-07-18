// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BulletSpec.h"
#include "ProjectileUtilities.generated.h"

class UFPSAbilitySystemComponent;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UProjectileUtilities : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    static FBulletSpec MakeBulletSpec(const UFPSAbilitySystemComponent* ASC);
};
