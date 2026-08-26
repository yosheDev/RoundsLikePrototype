// Copyright Jacob Jones 2026

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Drafting/AllocationWidgetIDInterface.h"
#include "DraftStatButton.generated.h"

class UButton;

UCLASS()
class ROUNDSLIKEPROTOTYPE_API UDraftStatButton : public UUserWidget, public IAllocationWidgetIDInterface
{
	GENERATED_BODY()

public:

	virtual int32 GetWidgetID_Implementation() override;

	virtual void SetWidgetID_Implementation(int32 NewID) override;

	int32 WidgetID = -1;

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StatButton;
	
protected:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ClickButton();

private:

	bool bIsAllocated = false;

	// This should never not be 1, but building it this way anyways in case I change mind.
	UPROPERTY(EditAnywhere)
	uint8 Cost = 1;

	UFUNCTION()
	bool TryAllocation();

	UFUNCTION()
	bool TryDeallocation();
};
