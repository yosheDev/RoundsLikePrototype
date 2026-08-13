// Copyright Jacob Jones 2026


#include "Abilities/AbilityDefinition.h"

FPrimaryAssetId UAbilityDefinition::GetPrimaryAssetID() const
{
    return FPrimaryAssetId(
        FPrimaryAssetType(TEXT("AbilityDefinition")),
        AbilityTag.GetTagName()
    );
}
