// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "Net/UnrealNetwork.h"

UGunplayAttributeSet::UGunplayAttributeSet()
{
	//InitBulletGravity(1.0f);
	//InitBulletSpeed(4000.0f);
	//InitBulletImpactDamage(25.0f);
	//InitAutoFireRate(5.0f); // Bullets
}

void UGunplayAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGunplayAttributeSet, BulletJumpFactor);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSpeed);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSize);
	DOREPLIFETIME(UGunplayAttributeSet, BulletArc);
	DOREPLIFETIME(UGunplayAttributeSet, BulletGravity);
	DOREPLIFETIME(UGunplayAttributeSet, BulletCritChance);
	DOREPLIFETIME(UGunplayAttributeSet, BulletLifestealAmount);
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