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

	UMeshComponent* Mesh = nullptr;
	TArray<UMeshComponent*> Components;
	GetOwner()->GetComponents<UMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++)
	{
		Mesh = Components[i];
		break;
	}

	if (Mesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't get owner's mesh: %s"), *GetName());
		return;
	}

	Mesh->OnBeginCursorOver.AddUniqueDynamic(this, &UClickInteractComponent::ActivateHighlight);
	Mesh->OnEndCursorOver.AddUniqueDynamic(this, &UClickInteractComponent::DeactivateHighlight);
	// ...
}

// Called every frame
void UClickInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UClickInteractComponent::ActivateHighlight(UPrimitiveComponent* TouchedComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBeginCursorOver: %s"), *GetOwner()->GetName());

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AGravityCharacter* Player = Cast<AGravityCharacter>(PlayerPawn);
	if (Player == nullptr || (!Player->HasRelic1() && !Player->HasRelic2()) || bSelected)
	{
		return;
	}

	bClickable = true;

	// has no ability to swap gravity between player + obj 
	if (!Player->HasRelic1())
	{
		if (Player->GetClickFocusType(Player->GetCurrentClickFocus()) == FocusType::Player && Player->GetClickFocusType(this) == FocusType::Object
			|| Player->GetClickFocusType(Player->GetCurrentClickFocus()) == FocusType::Object && Player->GetClickFocusType(this) == FocusType::Player)
		{
			bClickable = false;
		}
	}

	// has no ability to swap gravity between obj + obj 
	if (!Player->HasRelic2())
	{
		if (Player->GetClickFocusType(Player->GetCurrentClickFocus()) == FocusType::Object && Player->GetClickFocusType(this) == FocusType::Object)
		{
			bClickable = false;
		}
	}

	// if this component CANNOT swap gravity with player's current focus
	if (Player->GetCurrentClickFocus() != nullptr)
	{
		if (Player->CanSwapGravity(this, Player->GetCurrentClickFocus()) == false)
		{
			bClickable = false;
		}
	}

	bool bIsWithinRange = Player->GetDistanceTo(GetOwner()) < Player->GetClickInteractRange();
	OnActivateHighlight.Broadcast(bIsWithinRange, bClickable);
}

void UClickInteractComponent::DeactivateHighlight(UPrimitiveComponent* TouchedComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("OnEndCursorOver: %s"), *GetOwner()->GetName());
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AGravityCharacter* Player = Cast<AGravityCharacter>(PlayerPawn);
	if (Player == nullptr || (!Player->HasRelic1() && !Player->HasRelic2()) || bSelected)
	{
		return;
	}

	bool bIsWithinRange = Player->GetDistanceTo(GetOwner()) < Player->GetClickInteractRange();
	OnDeactivateHighlight.Broadcast(bIsWithinRange, bClickable);
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

