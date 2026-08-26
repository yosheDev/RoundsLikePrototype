// Copyright Jacob Jones 2026


#include "Components/MatchEconomyComponent.h"
#include "Components/BottlecapAllocationData.h"
#include "FPSHudController.h"
#include "FPSPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

UMatchEconomyComponent::UMatchEconomyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UMatchEconomyComponent::BeginPlay()
{
	Super::BeginPlay();

    InitializeBottlecapAllocations();
}

void UMatchEconomyComponent::InitializeBottlecapAllocations()
{
    BottlecapAllocations.Empty();
    for (uint8 i = 0; i < MaxBottlecaps; i++)
    {
        BottlecapAllocations.Add(false);
    }

    AvailableBottlecapIndices.Empty();
    for (uint8 i = 0; i < MaxBottlecaps; i++)
    {
        AvailableBottlecapIndices.Add(i);
    }

    SetBottlecaps(MaxBottlecaps);
}

void UMatchEconomyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UMatchEconomyComponent, CurrentAbilityOffers);
}

void UMatchEconomyComponent::OnRep_CurrentAbilityOffers()
{
    if (!PC)
    {
        PC = GetPlayerController();
        if (!PC) { return; }

        AFPSHudController* HUD = PC->GetHUD<AFPSHudController>();

        HUD->RefreshAbilitySelection();
    }
}

void UMatchEconomyComponent::SetBottlecaps(uint8 Amount)
{
	AvailableBottlecaps = Amount;
}

bool UMatchEconomyComponent::CanAllocateBottlecaps(uint8 Amount)
{
    uint8 AvailableBottlecapsAmount = 0;
    TArray<uint8> NewAvailableBottlecapIndices;

    for (int32 i = 0; i < BottlecapAllocations.Num(); i++)
    {
        // If is not allocated.
        if (!BottlecapAllocations[i])
        {
            AvailableBottlecapsAmount++;
            NewAvailableBottlecapIndices.Add(i);
            UE_LOG(LogTemp, Log, TEXT("New AvailableBottlecapIndices: [%d]"), i);
        }
    }

    AvailableBottlecapIndices = NewAvailableBottlecapIndices;
    AvailableBottlecaps = AvailableBottlecapsAmount;

    if (AvailableBottlecapsAmount >= Amount)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void UMatchEconomyComponent::Server_AllocateBottlecaps_Implementation(uint8 Amount, int32 WidgetID, const TArray<FVector2D>& AllocationLocations)
{
    AllocateBottlecaps(Amount, WidgetID, AllocationLocations);
}

void UMatchEconomyComponent::AllocateBottlecaps(uint8 Amount, int32 WidgetID, const TArray<FVector2D>& AllocationLocations)
{
    // Clients call the Server RPC.
    if (!(GetOwner()->HasAuthority()))
    {
        Server_AllocateBottlecaps(Amount, WidgetID, AllocationLocations);
        return;
    }

    // Allocation Variables
    uint8 AvailableBottlecapsAmount = 0;
    TArray<uint8> ReturnAvailableBottlecapIndices = AvailableBottlecapIndices;
    TArray<int32> IndicesToAllocate;


    // Allocate from available indices.
    for (int32 i = 0; i < Amount; i++)
    {
        UE_LOG(LogTemp, Log, TEXT("i in Amount Loop: [%d]"), i);
        if (AvailableBottlecapIndices.IsValidIndex(i))
        {
            UE_LOG(LogTemp, Log, TEXT("Available Indices: [%d]"), AvailableBottlecapIndices[i]);
            IndicesToAllocate.Add(AvailableBottlecapIndices[i]);
        }
    }

    // Create new allocation data. (WidgetID tied with the indices it has allocated to it.)
    FBottlecapAllocationData NewAllocationData = FBottlecapAllocationData(WidgetID, IndicesToAllocate);
    AllocationData.Add(NewAllocationData.WidgetID, NewAllocationData);

    for (int32 i = 0; i < IndicesToAllocate.Num(); i++)
    {
        BottlecapAllocations[IndicesToAllocate[i]] = true;
        AvailableBottlecapIndices.Remove(IndicesToAllocate[i]);
        AvailableBottlecaps--;

        Multicast_UpdateBottlecapHUD(IndicesToAllocate[i], AllocationLocations[i]);
    }
}

void UMatchEconomyComponent::Server_DeallocateBottlecaps_Implementation(int32 WidgetID)
{
    DeallocateBottlecaps(WidgetID);
}

void UMatchEconomyComponent::DeallocateBottlecaps(int32 WidgetID)
{
    // Clients call the Server RPC.
    if (!(GetOwner()->HasAuthority()))
    {
        Server_DeallocateBottlecaps(WidgetID);
        return;
    }

    // Get HUD Reference
    if (!PC)
    {
        PC = GetPlayerController();
        if (!PC) { return; }
    }
    AFPSHudController* HUD = PC->GetHUD<AFPSHudController>();

    TArray<FVector2D> ReturnLocations;

    // Get deallocation indices by WidgetID.
    if (FBottlecapAllocationData* DataPtr = AllocationData.Find(WidgetID))
    {
        FBottlecapAllocationData Data = *DataPtr;

        if (HUD)
        {
            ReturnLocations = HUD->GetBottlecapReturnLocations((uint8)(Data.AllocatedIndices).Num());
        }

        for (int32 i = 0; i < Data.AllocatedIndices.Num(); i++)
        {
            // Deallocate the indices.
            BottlecapAllocations[Data.AllocatedIndices[i]] = false;
            AvailableBottlecapIndices.Add(Data.AllocatedIndices[i]);
            AvailableBottlecaps++;

            // Update DraftingUI NOTE NEED TO MAKE FUNCTION TO GET LOCATION TO GO TO
            
            Multicast_UpdateBottlecapHUD(Data.AllocatedIndices[i], ReturnLocations[i], true);

            
        }

        // Remove entry from TMap AllocationData
        int32 RemovedAmount = AllocationData.Remove(Data.WidgetID);

        
    }
    else
    {
        // Key does not exist.
        UE_LOG(LogTemp, Error, TEXT("ERROR: Tried to deallocate bottlecaps using a WidgetID that is not a key in AllocationData."));
        return;
    }
}

void UMatchEconomyComponent::Multicast_UpdateBottlecapHUD_Implementation(uint8 BottlecapID, FVector2D AllocationLocation, bool bIsDeallocating = false)
{
    // UDraftingUI only needs to know which bottlecap is moving and where. Does not need to know anything else.
    
    // Get HUD Reference
    if (!PC)
    {
        PC = GetPlayerController();
        if (!PC) { return; }
    }
    AFPSHudController* HUD = PC->GetHUD<AFPSHudController>();

    if (HUD)
    {
        HUD->BeginTranslateBottlecap(BottlecapID, AllocationLocation, bIsDeallocating);
    }
}

APlayerController* UMatchEconomyComponent::GetPlayerController()
{
    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0);
        if (Controller && Controller->IsLocalPlayerController())
        {
            return Controller;
        }
    }
    
    return nullptr;  
}

