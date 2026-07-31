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

    UE_LOG(LogTemp, Warning,
        TEXT("MYTEST Bullet ASC Owner: %s"),
        *GetNameSafe(ASC->GetOwner()));

    UE_LOG(LogTemp, Warning,
        TEXT("MYTEST Bullet Gunplay Set: %s"),
        *GetNameSafe(Attributes));

    UE_LOG(LogTemp, Warning,
        TEXT("MYTEST Bullet Speed: %f"),
        Attributes->GetBulletSpeed());

    Spec.BulletSpeed = Attributes->GetBulletSpeed();
    Spec.BulletGravity = Attributes->GetBulletGravity();

    ASC->GetOwnedGameplayTags(Spec.Tags);

    return Spec;
}
