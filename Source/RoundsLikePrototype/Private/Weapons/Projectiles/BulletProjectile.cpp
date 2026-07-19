// Copyright Jacob Jones 2026


#include "Weapons/Projectiles/BulletProjectile.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/Projectiles/BulletSpec.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Components/FPSAbilitySystemComponent.h"

// Sets default values
ABulletProjectile::ABulletProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	#pragma region Create Components
	SphereHitCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereHitCollision"));
	SphereHitCollision->SetNotifyRigidBodyCollision(true);
	SetRootComponent(SphereHitCollision);

	SphereOverlapCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereOverlapCollision"));
	SphereOverlapCollision->SetupAttachment(RootComponent);
	SphereOverlapCollision->SetGenerateOverlapEvents(true);

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
	
	// Binds Sphere Component collision methods to this .cpp scripts equivalent function.
	SphereHitCollision->OnComponentHit.AddDynamic(this, &ABulletProjectile::OnComponentHit);
	SphereOverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &ABulletProjectile::OnComponentBeginOverlapEvent);

	NiagaraComponent->Activate(true);
}

void ABulletProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABulletProjectile, BulletSpec);
}

// Called every frame
void ABulletProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABulletProjectile::OnRep_BulletSpec()
{
	InitializeBulletSpec(BulletSpec);
}

void ABulletProjectile::InitializeBulletSpec(FBulletSpec InBulletSpec)
{
	BulletSpec = InBulletSpec;
	ProjectileMovementComponent->Velocity = GetInstigator()->GetActorForwardVector() * BulletSpec.BulletSpeed;
	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("[%f] Bullet Spec Speed"), BulletSpec.BulletSpeed));
	ProjectileMovementComponent->InitialSpeed = BulletSpec.BulletSpeed;
	ProjectileMovementComponent->MaxSpeed = TNumericLimits<float>::Max();
	ProjectileMovementComponent->ProjectileGravityScale = BulletSpec.BulletGravity;

	ProjectileMovementComponent->bAutoActivate = true;
	ProjectileMovementComponent->Activate();
}

void ABulletProjectile::InitializeGameplayEffectSpec(FGameplayEffectSpecHandle InEffectSpec)
{
	GameplayEffectSpec = InEffectSpec;
}


void ABulletProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		Destroy();
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("Projectile Hit: %s"), *OtherActor->GetName()));

	// Get the targets ASC and apply the delivered projectile gameplay effect.
	UAbilitySystemComponent* TargetASC = nullptr;
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		TargetASC = ASCInterface->GetAbilitySystemComponent();

		if (TargetASC && GameplayEffectSpec.IsValid())
		{
			FGameplayEffectContextHandle Context = GameplayEffectSpec.Data->GetContext();
			Context.AddHitResult(Hit);
			GameplayEffectSpec.Data->SetContext(Context);

			TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpec.Data);
			GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("Apply Projectile Gameplay Effect")));
		}
	}	

	Destroy();
}

void ABulletProjectile::OnComponentBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && (OtherActor != GetInstigator()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("Overlapped with: %s"), *OtherActor->GetName()));

		// Add your custom gameplay logic here (e.g., damage, pickup, text prints)
	}
}