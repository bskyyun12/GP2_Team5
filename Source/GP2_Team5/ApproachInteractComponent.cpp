// Fill out your copyright notice in the Description page of Project Settings.


#include "ApproachInteractComponent.h"

// Sets default values for this component's properties
UApproachInteractComponent::UApproachInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UApproachInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UApproachInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UApproachInteractComponent::Interact_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: Interact()"), *GetName());
	OnInteract.Broadcast();
}

void UApproachInteractComponent::ShowInteractionWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: ShowInteractionWidget()"), *GetName());
}

void UApproachInteractComponent::HideInteractionWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: HideInteractionWidget()"), *GetName());
}
