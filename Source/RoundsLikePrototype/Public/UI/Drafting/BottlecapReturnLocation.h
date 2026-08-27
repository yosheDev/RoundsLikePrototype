// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "BottlecapReturnLocation.generated.h"

USTRUCT(BlueprintType)
struct FBottlecapReturnLocation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 SlotIndex = -1;

	UPROPERTY(BlueprintReadWrite)
	FVector2D Location = FVector2D::ZeroVector;
};
