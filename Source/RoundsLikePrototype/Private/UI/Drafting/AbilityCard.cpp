// Copyright Jacob Jones 2026


#include "UI/Drafting/AbilityCard.h"
#include "GameplayTagContainer.h"
#include "FPSGameState.h"
#include "FPSPlayerState.h"
#include "Abilities/AbilityDefinition.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UAbilityCard::NativeConstruct()
{
    Super::NativeConstruct();

    if (SelectAbilityButton)
    {
        SelectAbilityButton->OnClicked.AddDynamic(this, &UAbilityCard::SelectAbility);
    }

    AbilityName->SetText(AbilityDataAsset->Name);
    AbilityDesc->SetText(AbilityDataAsset->Description);
}

void UAbilityCard::SelectAbility()
{
    if (!bAlreadySelected)
    {
        bAlreadySelected = true;
        GiveAbilityToPlayer();
    }
}

void UAbilityCard::GiveAbilityToPlayer()
{
    AFPSGameState* GS = GetWorld()->GetGameState<AFPSGameState>();

    if (!GS)
    {
        return;
    }

    AFPSPlayerState* LoserState = GS->GetCurrentLoserState();

    if (!LoserState)
    {
        return;
    }
    
    LoserState->Server_AddAccruedAbility_Implementation(AbilityDataAsset->AbilityTag);
}

