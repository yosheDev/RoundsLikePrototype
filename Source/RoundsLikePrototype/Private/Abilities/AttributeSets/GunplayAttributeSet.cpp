// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AttributeSets/GunplayAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UGunplayAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGunplayAttributeSet, BulletJumpFactor);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSpeed);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSize);
	DOREPLIFETIME(UGunplayAttributeSet, BulletArc);
	DOREPLIFETIME(UGunplayAttributeSet, BulletCritChance);
	DOREPLIFETIME(UGunplayAttributeSet, BulletLifestealAmount);
	DOREPLIFETIME(UGunplayAttributeSet, BulletType);
	DOREPLIFETIME(UGunplayAttributeSet, BulletAmount);
	DOREPLIFETIME(UGunplayAttributeSet, BulletSpreadType);
	DOREPLIFETIME(UGunplayAttributeSet, BulletRegen);
	DOREPLIFETIME(UGunplayAttributeSet, ClipCapacity);
	DOREPLIFETIME(UGunplayAttributeSet, RecoilFactor);
}