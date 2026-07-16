// Copyrighted Jacob Jones 2026

#include "Weapons/ProjectileWeapon.h"

// Constructor
AProjectileWeapon::AProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AProjectileWeapon::CanFire() const
{
	return true;
}

TArray<FTransform> AProjectileWeapon::GetMuzzleLocations() const
{
	return TArray<FTransform>();
}

void AProjectileWeapon::PrimaryFire()
{

}

