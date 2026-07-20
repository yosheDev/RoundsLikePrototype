// Copyright Jacob Jones 2026


#include "Weapons/Projectiles/BulletProjectile.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/Projectiles/BulletSpec.h"
#include "Weapons/Projectiles/ProjectileSpawnData.h"
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
	if (!HasAuthority())
	{
		SetReplicates(false);
	}
	else 
	{
		SetReplicates(true);
	}
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
	
	if (!HasAuthority() && !bPredictedProjectile)
	{
		// This is flawed, it makes client unable to see the servers real projectiles as well...
		SetActorHiddenInGame(true);
	}

	// Binds Sphere Component collision methods to this .cpp scripts equivalent function.
	SphereHitCollision->OnComponentHit.AddDynamic(this, &ABulletProjectile::OnComponentHit);
	SphereOverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &ABulletProjectile::OnComponentBeginOverlapEvent);

	NiagaraComponent->Activate(true);
}

void ABulletProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABulletProjectile, BulletData);
}

// Called every frame
void ABulletProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABulletProjectile::OnRep_BulletData()
{
	InitializeBulletData(BulletData);
}

void ABulletProjectile::InitializeBulletData(FProjectileSpawnData InBulletData)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeBulletSpec Authority=%s"),
		*GetName(),
		HasAuthority() ? TEXT("YES") : TEXT("NO"));

	BulletData = InBulletData;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
	ProjectileMovementComponent->InitialSpeed = 0.0f;
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * InBulletData.BulletSpec.BulletSpeed;
	ProjectileMovementComponent->InitialSpeed = InBulletData.BulletSpec.BulletSpeed;
	ProjectileMovementComponent->MaxSpeed = TNumericLimits<float>::Max();
	ProjectileMovementComponent->ProjectileGravityScale = InBulletData.BulletSpec.BulletGravity;

	ProjectileMovementComponent->bAutoActivate = true;
	ProjectileMovementComponent->Activate();
}

void ABulletProjectile::InitializeGameplayEffectSpec(FGameplayEffectSpecHandle InEffectSpec)
{
	GameplayEffectSpec = InEffectSpec;
}


void ABulletProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/*if (!HasAuthority())
	{
		Destroy();
		return;
	}*/

	Destroy();
}

void ABulletProjectile::OnComponentBeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator())
	{
		return;
	}

	if (bPredictedProjectile)
	{
		PredictDamage(OtherActor);
		Destroy();
		return;
	}

	if (HasAuthority())
	{
		ApplyDamage(OtherActor);
		Destroy();
	}

	//#pragma region Client Predicts Damage
	//if (!HasAuthority())
	//{
	//	if (OtherActor && (OtherActor != this) && (OtherActor != GetInstigator()))
	//	{
	//		// Call predicition health thing on the other actor
	//	}
	//	Destroy();
	//	return;
	//}
	//#pragma endregion

	//#pragma region Server Applys Damage
	//if (OtherActor && (OtherActor != this) && (OtherActor != GetInstigator()))
	//{
	//	//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("Overlapped with: %s"), *OtherActor->GetName()));

	//	// Get the targets ASC and apply the delivered projectile gameplay effect.
	//	UAbilitySystemComponent* TargetASC = nullptr;
	//	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	//	{
	//		TargetASC = ASCInterface->GetAbilitySystemComponent();

	//		if (TargetASC && GameplayEffectSpec.IsValid())
	//		{
	//			//FGameplayEffectContextHandle Context = GameplayEffectSpec.Data->GetContext();
	//			//Context.AddHitResult(Hit); // Hit not really something can do for Overlaps.
	//			//GameplayEffectSpec.Data->SetContext(Context);

	//			TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpec.Data);
	//			//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("Apply Projectile Gameplay Effect")));
	//		}
	//	}

	//	Destroy();
	//}
	//#pragma endregion
}

void ABulletProjectile::PredictDamage(AActor* Target)
{
	// Get ASC Properly
	IAbilitySystemInterface* TargetInterface = Cast<IAbilitySystemInterface>(Target);
	if (!TargetInterface) { return; }
	UAbilitySystemComponent* TargetASC = TargetInterface->GetAbilitySystemComponent();
	if (!TargetASC) { return; }
	UAbilitySystemComponent* SourceASC = Cast<IAbilitySystemInterface>(GetInstigator())->GetAbilitySystemComponent();
	if (!SourceASC) { return; }

	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.AddSourceObject(this);

	FScopedPredictionWindow PredictionWindow(SourceASC, true);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(ProjectileGameplayEffect, 1, Context);

	if (SpecHandle.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ABulletProjectile::ApplyDamage(AActor* Target)
{
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Target);

	if (!ASCInterface)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();

	if (TargetASC && GameplayEffectSpec.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpec.Data);
	}
}