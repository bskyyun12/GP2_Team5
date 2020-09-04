// Fill out your copyright notice in the Description page of Project Settings.


#include "ClickInteractComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GravityCharacter.h"
#include "Enums.h"

// Sets default values for this component's properties
UClickInteractComponent::UClickInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;



}


// Called when the game starts
void UClickInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	UMeshComponent* Mesh = GetMeshComponent<USkeletalMeshComponent>(GetOwner());
	if (Mesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't Find SkeletalMeshComp. Trying to get StaticMeshComp..."));
		Mesh = GetMeshComponent<UStaticMeshComponent>(GetOwner());
		if (Mesh == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't Find StaticMeshComp either..."));
			return;
		}
	}

	if (Mesh != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Mesh! Owner: %s, Mesh: %s"), *GetOwner()->GetName(), *Mesh->GetName());
		Mesh->OnBeginCursorOver.AddUniqueDynamic(this, &UClickInteractComponent::ActivateHighlight);
		Mesh->OnEndCursorOver.AddUniqueDynamic(this, &UClickInteractComponent::DeactivateHighlight);
	}
	// ...
}

// Called every frame
void UClickInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UClickInteractComponent::Clickable()
{
	AGravityCharacter* Player = GetGravityCharacter();

	// has no ability to swap gravity between player + obj 
	if (!Player->HasRelic1())
	{
		if (Player->GetClickFocusType(Player->GetCurrentClickFocus()) == EFocusType::Player && Player->GetClickFocusType(this) == EFocusType::Object
			|| Player->GetClickFocusType(Player->GetCurrentClickFocus()) == EFocusType::Object && Player->GetClickFocusType(this) == EFocusType::Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player doesn't have Relic1"));
			return false;
		}
	}

	// has no ability to swap gravity between obj + obj 
	if (!Player->HasRelic2())
	{
		if (Player->GetClickFocusType(Player->GetCurrentClickFocus()) == EFocusType::Object && Player->GetClickFocusType(this) == EFocusType::Object)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player doesn't have Relic2"));
			return false;
		}
	}

	// if this component CANNOT swap gravity with player's current focus
	if (Player->GetCurrentClickFocus() != nullptr)
	{
		if (Player->CanSwapGravity(this, Player->GetCurrentClickFocus()) == false)
		{
			return false;
		}
	}

	// this component is not in player's line of sight
	if (Player->IsComponentInLineOfSight(this) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("This actor ( %s ) is not in player's line of sight"), *GetOwner()->GetName());
		return false;
	}

	// This component is not within player's interact range
	if (Player->GetDistanceTo(GetOwner()) > Player->GetClickInteractRange())
	{
		UE_LOG(LogTemp, Warning, TEXT("This actor ( %s ) is not within Player's interaction range"), *GetOwner()->GetName());
		return false;
	}

	return true;
}

AGravityCharacter* UClickInteractComponent::GetGravityCharacter()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return Cast<AGravityCharacter>(PlayerPawn);
}

void UClickInteractComponent::OnReset()
{
	bSelected = false;
	bIsSwappableColorOn = false;
	IClickInteract::Execute_ResetClickInteract(this);
	DeactivateHighlight(nullptr);
}

void UClickInteractComponent::ActivateHighlight(UPrimitiveComponent* TouchedComponent)
{
	AGravityCharacter* Player = GetGravityCharacter();
	if (Player == nullptr || (!Player->HasRelic1() && !Player->HasRelic2()) || bSelected || bIsSwappableColorOn)
	{
		return;
	}

	bool bIsWithinRange = Player->GetDistanceTo(GetOwner()) < Player->GetClickInteractRange();
	OnActivateHighlight.Broadcast(bIsWithinRange, Clickable() ? HoverColor : NonClickableColor);
}

void UClickInteractComponent::SwappableHighlight()
{
	AGravityCharacter* Player = GetGravityCharacter();
	if (Player == nullptr || (!Player->HasRelic1() && !Player->HasRelic2()) || bSelected)
	{
		return;
	}

	bIsSwappableColorOn = true;

	bool bIsWithinRange = Player->GetDistanceTo(GetOwner()) < Player->GetClickInteractRange();
	OnActivateHighlight.Broadcast(bIsWithinRange, SwappableColor);
}

void UClickInteractComponent::DeactivateHighlight(UPrimitiveComponent* TouchedComponent)
{
	AGravityCharacter* Player = GetGravityCharacter();
	if (Player == nullptr || (!Player->HasRelic1() && !Player->HasRelic2()) || bSelected || bIsSwappableColorOn)
	{
		return;
	}

	bool bIsWithinRange = Player->GetDistanceTo(GetOwner()) < Player->GetClickInteractRange();
	OnDeactivateHighlight.Broadcast(bIsWithinRange, Clickable());
}

void UClickInteractComponent::ClickInteract_Implementation()
{
	OnClickInteract.Broadcast();
	//UE_LOG(LogTemp, Warning, TEXT("ClickInteract: %s"), *this->GetName());
}

void UClickInteractComponent::ResetClickInteract_Implementation()
{
	OnResetClickInteract.Broadcast();
	//UE_LOG(LogTemp, Warning, TEXT("ResetClickInteract: %s"), *this->GetName());
}

