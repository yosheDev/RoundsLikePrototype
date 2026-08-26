// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AllocationWidgetIDInterface.generated.h"

// This class is managed by the UObject system. Do not modify it directly.
UINTERFACE(MinimalAPI, Blueprintable)
class UAllocationWidgetIDInterface : public UInterface
{
	GENERATED_BODY()
};

class ROUNDSLIKEPROTOTYPE_API IAllocationWidgetIDInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
	int32 GetWidgetID();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI")
	void SetWidgetID(int32 NewID);
};
