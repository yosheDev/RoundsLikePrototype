// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "Net/UnrealNetwork.h"

UGunplayAttributeSet::UGunplayAttributeSet()
{
	InitBulletGravity(1.0f);
	InitBulletSpeed(4000.0f);
	InitBulletImpactDamage(100.0f);
	InitAutoFireRate(5.0f); // Bullets/s
}

void UGunplayAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGunplayAttributeSet, BulletJumpFactor);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSpeed);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSize);
	DOREPLIFETIME(UGunplayAttributeSet, BulletGravity);
	DOREPLIFETIME(UGunplayAttributeSet, BulletCritChance);
	DOREPLIFETIME(UGunplayAttributeSet, BulletLifestealAmount);
	DOREPLIFETIME(UGunplayAttributeSet, BulletType);
	DOREPLIFETIME(UGunplayAttributeSet, BulletAmount);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSpreadType);
	DOREPLIFETIME(UGunplayAttributeSet, BulletRegen);
	DOREPLIFETIME(UGunplayAttributeSet, AutoFireRate);
	DOREPLIFETIME(UGunplayAttributeSet, ClipCapacity);
	DOREPLIFETIME(UGunplayAttributeSet, RecoilFactor);
}