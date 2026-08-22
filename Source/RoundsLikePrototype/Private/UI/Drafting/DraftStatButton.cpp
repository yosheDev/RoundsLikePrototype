// Copyright Jacob Jones 2026


#include "UI/Drafting/DraftStatButton.h"
#include "Components/Button.h"

void UDraftStatButton::NativeConstruct()
{
    Super::NativeConstruct();

    if (StatButton)
    {
        StatButton->OnClicked.AddDynamic(this, &UDraftStatButton::ClickButton);
    }
}

void UDraftStatButton::ClickButton_Implementation()
{

}

