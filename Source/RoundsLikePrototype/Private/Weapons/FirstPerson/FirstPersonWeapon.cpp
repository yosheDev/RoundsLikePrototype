// Copyright Jacob Jones 2026


#include "Weapons/FirstPerson/FirstPersonWeapon.h"
#include "Weapons/ProjectileWeapon.h"

// Sets default values
AFirstPersonWeapon::AFirstPersonWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(Mesh);
}

USkeletalMeshComponent* AFirstPersonWeapon::GetMesh()
{
	return Mesh;
}

void AFirstPersonWeapon::BeginPlay()
{
	Super::BeginPlay();

	Mesh->SetOnlyOwnerSee(true);
}

void AFirstPersonWeapon::BindToWeapon(TObjectPtr<AProjectileWeapon> Weapon)
{
	Weapon->OnPrimaryFire.AddDynamic(this, &AFirstPersonWeapon::PlayFireWeaponAnimation);
}

void AFirstPersonWeapon::PlayFireWeaponAnimation_Implementation()
{
	// Play appropriate montage based on what weapon the player has.
}

FVector AFirstPersonWeapon::GetProjectileSpawnLocation()
{
	FVector Location = FVector::Zero();

	if (!Mesh || !Mesh->DoesSocketExist(TEXT("ProjectileSpawnSocket")))
	{
		return Mesh ? Mesh->GetComponentTransform().GetLocation() : GetActorLocation();
	}

	Location = Mesh->GetSocketLocation(TEXT("ProjectileSpawnSocket"));

	return Location;
}
