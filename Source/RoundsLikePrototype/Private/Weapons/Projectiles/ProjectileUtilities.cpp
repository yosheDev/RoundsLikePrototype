// Copyright Jacob Jones 2026


#include "Weapons/Projectiles/ProjectileUtilities.h"
#include "Components/FPSAbilitySystemComponent.h"
#include "Abilities/AttributeSets/GunplayAttributeSet.h"

FBulletSpec UProjectileUtilities::MakeBulletSpec(const UFPSAbilitySystemComponent* ASC)
{
    FBulletSpec Spec;

    if (!ASC)
    {
        return Spec;
    }

    const UGunplayAttributeSet* Attributes = ASC->GetSet<UGunplayAttributeSet>();

    Spec.BulletDamage = Attributes->GetBulletDamage();
    Spec.BulletSpeed = Attributes->GetBulletSpeed();
    Spec.BulletGravity = Attributes->GetBulletGravity();

    ASC->GetOwnedGameplayTags(Spec.Tags);

    return Spec;
}
