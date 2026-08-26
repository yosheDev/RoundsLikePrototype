// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "BottlecapAllocationData.generated.h"

USTRUCT(BlueprintType)
struct FBottlecapAllocationData
{
    GENERATED_BODY()

    // Identifies the Widget storing the allocation
    UPROPERTY()
    int32 WidgetID = -1;

    // Stores which bottlecap indices the widget associated with WidgetID has allocated to it.
    UPROPERTY()
    TArray<int32> AllocatedIndices;
};
