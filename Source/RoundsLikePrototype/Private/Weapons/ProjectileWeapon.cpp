// Copyrighted Jacob Jones 2026

#include "Weapons/ProjectileWeapon.h"
#include "Weapons/Projectiles/BulletProjectile.h"
#include "Weapons/Projectiles/ProjectileUtilities.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Components/FPSAbilitySystemComponent.h"

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
	if (!HasAuthority())
	{
		return;
	}

	FString RoleString = HasAuthority() ? TEXT("Server") : TEXT("Client");
	//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("[%s] Weapon Primary Fire"), *RoleString));

	// Add logic for fire types i.e HasTag(Weapon.FireMode.Spread)

	#pragma region Spawn Projectile
	FVector Location(GetActorLocation());
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FTransform SpawnTransform(Rotation, Location);

	TObjectPtr<ABulletProjectile> Projectile = GetWorld()->SpawnActorDeferred<ABulletProjectile>(ProjectileClass, SpawnTransform, this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	#pragma region Initialize Projectile Specifiers (Damage, Speed, Size, Gravity, ETC.)
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		// Initialize Projectile Parameters(Things relevant to projectile movement, size, and traits.)
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();

		#pragma region Validation Checks
		if (!IsValid(ASC))
		{
			UE_LOG(LogTemp, Error, TEXT("ASC is invalid"));
			return;
		}

		if (!IsValid(ProjectileGameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("ProjectileGameplayEffect is invalid"));
			return;
		}
		#pragma endregion

		FBulletSpec Spec = UProjectileUtilities::MakeBulletSpec(Cast<UFPSAbilitySystemComponent>(ASC));
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, FString::Printf(TEXT("[%f] Bullet Spec Speed"), Spec.BulletSpeed));
		Projectile->InitializeBulletSpec(Spec);

		// Assign Gameplay Effect Spec
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		EffectContext.AddInstigator(GetInstigator(), this);
		FGameplayEffectSpecHandle GameplayEffectSpec = ASC->MakeOutgoingSpec(ProjectileGameplayEffect, 1, EffectContext);
		Projectile->GameplayEffectSpec = GameplayEffectSpec;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon was unable to retrieve the ASC. (Trying to initialize bullet spec)"));
	}
	#pragma endregion

	Projectile->FinishSpawning(SpawnTransform);

	#pragma endregion

	if (Projectile)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::White, FString::Printf(TEXT("[%s] Spawned Projectile"), *RoleString));
	}
}

