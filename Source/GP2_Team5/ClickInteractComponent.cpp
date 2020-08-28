// Fill out your copyright notice in the Description page of Project Settings.


#include "ClickInteractComponent.h"

// Sets default values for this component's properties
UClickInteractComponent::UClickInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UClickInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UClickInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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

