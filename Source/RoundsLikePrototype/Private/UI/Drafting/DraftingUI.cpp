// Copyright Jacob Jones 2026


#include "UI/Drafting/DraftingUI.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameMode.h"
#include "FPSPlayerController.h"
#include "Components/Button.h"

void UDraftingUI::NativeConstruct()
{
    Super::NativeConstruct();

    // Ensure the button pointer is valid before binding
    if (SelectDraftButton)
    {
        SelectDraftButton->OnClicked.AddDynamic(this, &UDraftingUI::HandleMyButtonClick);
    }
}

void UDraftingUI::HandleMyButtonClick()
{
    if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetOwningPlayer()))
    {
        PC->Server_FinishedDraft();
    } 
}
