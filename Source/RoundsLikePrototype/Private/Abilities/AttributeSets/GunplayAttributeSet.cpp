// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "Net/UnrealNetwork.h"

UGunplayAttributeSet::UGunplayAttributeSet()
{
	// Values init from data table.
}

void UGunplayAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	#pragma region Bullet Damage
	DOREPLIFETIME(UGunplayAttributeSet, BulletImpactDamage);

	DOREPLIFETIME(UGunplayAttributeSet, BulletDamageFalloffStartDistance);
	DOREPLIFETIME(UGunplayAttributeSet, BulletDamageFalloffEndDistance);
	DOREPLIFETIME(UGunplayAttributeSet, BulletDamageFarModifier);

	DOREPLIFETIME(UGunplayAttributeSet, BulletDamageReverseFalloffStartDistance);
	DOREPLIFETIME(UGunplayAttributeSet, BulletDamageReverseFalloffEndDistance);
	DOREPLIFETIME(UGunplayAttributeSet, BulletDamageNearModifier);
	#pragma endregion

	DOREPLIFETIME(UGunplayAttributeSet, BulletJumpFactor);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSpeed);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSize);
	DOREPLIFETIME(UGunplayAttributeSet, BulletArc);
	DOREPLIFETIME(UGunplayAttributeSet, BulletArcDistance);
	DOREPLIFETIME(UGunplayAttributeSet, BulletArcPitchInfluence);
	DOREPLIFETIME(UGunplayAttributeSet, BulletGravity);
	DOREPLIFETIME(UGunplayAttributeSet, BulletCritChance);
	DOREPLIFETIME(UGunplayAttributeSet, BulletLifestealAmount);
	DOREPLIFETIME(UGunplayAttributeSet, BulletBounceAmount);
	DOREPLIFETIME(UGunplayAttributeSet, BulletBounceVelocityRetention);
	DOREPLIFETIME(UGunplayAttributeSet, BulletType);

	DOREPLIFETIME(UGunplayAttributeSet, WeaponFireType);
	DOREPLIFETIME(UGunplayAttributeSet, ClipCapacity);
	DOREPLIFETIME(UGunplayAttributeSet, AutoFireRate);
	DOREPLIFETIME(UGunplayAttributeSet, FireShotAmount);
	DOREPLIFETIME(UGunplayAttributeSet, FireBurstAmount);
	DOREPLIFETIME(UGunplayAttributeSet, BulletRegen);
	DOREPLIFETIME(UGunplayAttributeSet, BurstFireRate);
	DOREPLIFETIME(UGunplayAttributeSet, FireSpreadXAngle);
	DOREPLIFETIME(UGunplayAttributeSet, FireSpreadZAngle);
	DOREPLIFETIME(UGunplayAttributeSet, RecoilFactor);
}