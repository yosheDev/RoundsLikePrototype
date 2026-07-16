// Copyrighted Jacob Jones 2026

#include "Weapons/ProjectileWeapon.h"

// Constructor
AProjectileWeapon::AProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
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
	FString RoleString = HasAuthority() ? TEXT("Server") : TEXT("Client");
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("[%s] Weapon Primary Fire"), *RoleString));
}

