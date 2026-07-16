// Copyrighted Jacob Jones 2026


#include "Components/FPSAbilitySystemComponent.h"

UFPSAbilitySystemComponent::UFPSAbilitySystemComponent()
{

}

void UFPSAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
    /* 
        Server: Runs when GiveAbility() is called.
        Client: Runs after GAS replicates a GameplayAbilitySpec.
    */
	#pragma region Get Network Role String
	ENetRole LocalRole = GetOwner()->GetLocalRole();
	FString RoleString = TEXT("Unknown");

	switch (LocalRole)
	{
	case ROLE_Authority:
		RoleString = (GetWorld()->IsNetMode(NM_Client)) ? TEXT("Server (Autonomous)") : TEXT("Server (Authority)");
		break;
	case ROLE_AutonomousProxy:
		RoleString = TEXT("Client (Autonomous Proxy)");
		break;
	case ROLE_SimulatedProxy:
		RoleString = TEXT("Client (Simulated Proxy)");
		break;
	case ROLE_None:
		RoleString = TEXT("None");
		break;
	}
	#pragma endregion
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Purple, FString::Printf(TEXT("%s OnGiveAbility()"), *RoleString));

    Super::OnGiveAbility(AbilitySpec);
    OnAbilitySpecRecieved.Broadcast(AbilitySpec);
}

void UFPSAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

