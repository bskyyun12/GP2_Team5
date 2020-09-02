// Fill out your copyright notice in the Description page of Project Settings.


#include "ApproachInteractComponent.h"

// Sets default values for this component's properties
UApproachInteractComponent::UApproachInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UApproachInteractComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UApproachInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UApproachInteractComponent::Interact_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: Interact()"), *GetName());
	OnInteract.Broadcast();
}

void UApproachInteractComponent::InteractReleased_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: InteractReleased()"), *GetName());
	OnInteractReleased.Broadcast();
}

void UApproachInteractComponent::ShowInteractionWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: ShowInteractionWidget()"), *GetName());
}

void UApproachInteractComponent::HideInteractionWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: HideInteractionWidget()"), *GetName());
}
