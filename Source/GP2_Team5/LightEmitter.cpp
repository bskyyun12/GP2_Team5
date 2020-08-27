// Fill out your copyright notice in the Description page of Project Settings.


#include "LightEmitter.h"
#include "Engine/World.h"

#include "DrawDebugHelpers.h"

// Sets default values
ALightEmitter::ALightEmitter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALightEmitter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALightEmitter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Start = GetActorLocation();

	float DistanceFromCenter = Start.Size();
	Start.Normalize();

	auto angle = FMath::Atan2(Start.Y, Start.Z);

	QuantizationLevel = FMath::Max(QuantizationLevel, 0.05f);

	for (float i = angle; i < PI + angle; i+= QuantizationLevel)
	{
		const float startY = FMath::Sin(i) * DistanceFromCenter;
		const float startZ = FMath::Cos(i) * DistanceFromCenter;
		const float endY = FMath::Sin(i+ QuantizationLevel) * DistanceFromCenter;
		const float endZ = FMath::Cos(i+ QuantizationLevel) * DistanceFromCenter;

		const FVector start = { 0.0F, startY, startZ };
		const FVector end = { 0.0F, endY, endZ };

		DrawDebugLine(GetWorld(), start, end, FColor(255, 0, 0), false, 0.1f, 0, 3.f);
		FHitResult Hit;
		auto result = GetWorld()->LineTraceSingleByChannel(Hit, start, end, ECollisionChannel::ECC_Visibility);
		if (result)
		{
			UE_LOG(LogTemp, Warning, TEXT("we hit stuff"));
			break;
		}
	}
}
