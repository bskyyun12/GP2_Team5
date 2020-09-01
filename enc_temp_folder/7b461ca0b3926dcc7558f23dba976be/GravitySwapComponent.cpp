// Fill out your copyright notice in the Description page of Project Settings.


#include "GravitySwapComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UGravitySwapComponent::UGravitySwapComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = bCanEverTick;

	// ...
}

// Called when the game starts
void UGravitySwapComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	TArray<UStaticMeshComponent*> Components;
	GetOwner()->GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++)
	{
		Mesh = Components[i];
		break;
	}
}

// Called every frame
void UGravitySwapComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Mesh->IsSimulatingPhysics())
	{
		auto ClampedDeltaTime = FMath::Min(DeltaTime, 0.05f);
		auto GravityDirection = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(), GravityPoint);

		auto force = ClampedDeltaTime * GravityAcceleration * Mesh->GetMass();
		auto forceVector = GravityDirection * force;
		if (bFlipGravity)
		{
			Mesh->AddForce(GravityDirection * force * -1.f, NAME_None, true);
		}
		else
		{
			Mesh->AddForce(GravityDirection * force, NAME_None, true);
		}
	}
}

bool UGravitySwapComponent::GetFlipGravity() const
{
	return bFlipGravity;
}


void UGravitySwapComponent::SetFlipGravity(bool bNewGravity)
{
	GetOwner()->GetRootComponent()->ComponentVelocity = FVector::ZeroVector;
	bFlipGravity = bNewGravity;
	OnFlipGravity.Broadcast(bNewGravity);
}
