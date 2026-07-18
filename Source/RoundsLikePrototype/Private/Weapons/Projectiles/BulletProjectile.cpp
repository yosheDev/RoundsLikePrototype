// Copyright Jacob Jones 2026


#include "Weapons/Projectiles/BulletProjectile.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/Projectiles/BulletSpec.h"

// Sets default values
ABulletProjectile::ABulletProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	#pragma region Create Components
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bAutoActivate = false;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffectComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->bAutoActivate = false;
	#pragma endregion
}

// Called when the game starts or when spawned
void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	NiagaraComponent->Activate(true);
}

// Called every frame
void ABulletProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletProjectile::InitializeSpec(FBulletSpec BulletSpec)
{
	ProjectileMovementComponent->ProjectileGravityScale = BulletSpec.BulletGravity;
}

