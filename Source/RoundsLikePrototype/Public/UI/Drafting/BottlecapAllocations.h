// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "BottlecapAllocations.generated.h"

/* This can/should be deleted, it is not used anymore. */
USTRUCT(BlueprintType)
struct FBottlecapAllocations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Containers")
	TArray<TObjectPtr<UImage>> AllocatedBottlecaps;
};
