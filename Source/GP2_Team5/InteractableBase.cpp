// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBase.h"

// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = _RootComponent;

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Interaction Widget"));
	InteractionWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionWidget->SetVisibility(false);
}

// Called every frame
void AInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableBase::Interact_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: Interact()"), *GetName());
}

void AInteractableBase::ShowInteractionWidget()
{
	InteractionWidget->SetVisibility(true);
}

void AInteractableBase::HideInteractionWidget()
{
	InteractionWidget->SetVisibility(false);
}

