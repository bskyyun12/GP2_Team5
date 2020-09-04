// Fill out your copyright notice in the Description page of Project Settings.

#include "GravitySwapComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <../Plugins/Runtime/ApexDestruction/Source/ApexDestruction/Public/DestructibleComponent.h>

// Sets default values for this component's properties
UGravitySwapComponent::UGravitySwapComponent()
{
	PrimaryComponentTick.bCanEverTick = bCanEverTick;
}

// Called when the game starts
void UGravitySwapComponent::BeginPlay()
{
	SetComponentTickEnabled(bCanEverTick);
	Super::BeginPlay();

	TArray<UStaticMeshComponent*> Components;
	GetOwner()->GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++)
	{
		PhysicsComp = Components[i];
		return;
	}

	TArray<UDestructibleComponent*> DestructibleComponents;
	GetOwner()->GetComponents<UDestructibleComponent>(DestructibleComponents);
	for (int32 i = 0; i < DestructibleComponents.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found a destructible"));
		PhysicsComp = DestructibleComponents[i];
		return;
	}

}

// Called every frame
void UGravitySwapComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	if (PhysicsComp && PhysicsComp->IsSimulatingPhysics())
	{
		auto ClampedDeltaTime = FMath::Min(DeltaTime, 0.05f);
		auto GravityDirection = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(), GravityPoint);
		auto force = ClampedDeltaTime * GravityAcceleration * PhysicsComp->GetMass();
		auto forceVector = GravityDirection * force;

		if (bFlipGravity)
		{
			PhysicsComp->AddForce(GravityDirection * force * -1.f, NAME_None, true);
		}
		else
		{
			PhysicsComp->AddForce(GravityDirection * force, NAME_None, true);
		}
	}
}

bool UGravitySwapComponent::GetFlipGravity() const
{
	return bFlipGravity;
}


void UGravitySwapComponent::SetFlipGravity(bool bNewGravity)
{
	bFlipGravity = bNewGravity;
	OnFlipGravity.Broadcast(bNewGravity);
}
