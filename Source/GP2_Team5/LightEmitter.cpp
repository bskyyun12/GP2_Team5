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
	if (bIsCCW)
	{
		SendLaserCCW(GetActorLocation(), 0);
	}
	else
	{
		SendLaserCW(GetActorLocation(), 0);
	}
}


bool ALightEmitter::SendLaserCCW(FVector Start, int Bounces)
{
	if (Bounces > MaxBounces) return false;

	float DistanceFromCenter = Start.Size();
	Start.Normalize();

	auto angle = FMath::Atan2(Start.Y, Start.Z);

	QuantizationLevel = FMath::Max(QuantizationLevel, 0.05f);

	for (float i = angle; i < 2.F * PI + angle; i += QuantizationLevel)
	{
		const float StartY = FMath::Sin(i) * DistanceFromCenter;
		const float StartZ = FMath::Cos(i) * DistanceFromCenter;
		const float EndY = FMath::Sin(i + QuantizationLevel) * DistanceFromCenter;
		const float EndZ = FMath::Cos(i + QuantizationLevel) * DistanceFromCenter;

		const FVector StartPoint = { 0.0F, StartY, StartZ };
		const FVector EndPoint = { 0.0F, EndY, EndZ };

		DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor(255, 0, 0), false, 0.1f, 0, 3.f);
		FHitResult Hit;
		bool bHitSomething = GetWorld()->LineTraceSingleByChannel(Hit, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility);
		if (bHitSomething)
		{
			UE_LOG(LogTemp, Warning, TEXT("Send CW hit something"));

			float HitPlaneAngle = FVector::DotProduct(Hit.ImpactPoint.GetSafeNormal(), Hit.Normal);

			if (HitPlaneAngle > -0.707)
			{
				return SendLaserDown(Hit.ImpactPoint, Bounces + 1);
			}
			else
			{
				return SendLaserUp(Hit.ImpactPoint, Bounces + 1);
			}

		}
	}
	return false;
}

bool ALightEmitter::SendLaserCW(FVector Start, int Bounces)
{
	if (Bounces > MaxBounces) return false;

	float DistanceFromCenter = Start.Size();
	Start.Normalize();

	float Angle = FMath::Atan2(Start.Y, Start.Z);

	QuantizationLevel = FMath::Max(QuantizationLevel, 0.05f);

	for (float i = Angle; i > Angle - 2.F*PI; i -= QuantizationLevel)
	{
		const float StartY = FMath::Sin(i) * DistanceFromCenter;
		const float StartZ = FMath::Cos(i) * DistanceFromCenter;
		const float EndY = FMath::Sin(i - QuantizationLevel) * DistanceFromCenter;
		const float EndZ = FMath::Cos(i - QuantizationLevel) * DistanceFromCenter;

		const FVector StartPoint = { 0.0F, StartY, StartZ };
		const FVector EndPoint = { 0.0F, EndY, EndZ };

		DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor(255, 0, 0), false, 0.1f, 0, 3.f);
		FHitResult Hit;
		bool bHitSomething = GetWorld()->LineTraceSingleByChannel(Hit, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility);
		if (bHitSomething)
		{
			UE_LOG(LogTemp, Warning, TEXT("Send CW hit something"));

			float HitPlaneAngle = FVector::DotProduct(Hit.ImpactPoint.GetSafeNormal(), Hit.Normal);

			if (HitPlaneAngle > -0.707)
			{
				return SendLaserDown(Hit.ImpactPoint, Bounces + 1);
			}
			else
			{
				return SendLaserUp(Hit.ImpactPoint, Bounces + 1);
			}
			
		}
	}
	return false;
}

bool ALightEmitter::SendLaserUp(FVector Start, int Bounces)
{
	if (Bounces > MaxBounces) return false;
	FVector UpVector = Start.GetSafeNormal();

	FVector End = Start + 1000.F * UpVector;

	
	FHitResult Hit;
	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility);
	if (bHitSomething)
	{
		DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor(255, 0, 0), false, 0.1f, 0, 3.f);
		float HitPlaneAngle = FVector::DotProduct(Hit.ImpactPoint.GetSafeNormal(), Hit.Normal);

		UE_LOG(LogTemp, Warning, TEXT("Send up hit something angle : %f"), HitPlaneAngle);
		if (HitPlaneAngle >	-0.707F)
		{
			return SendLaserCCW(Hit.ImpactPoint, Bounces + 1);
		}
		else
		{
			return SendLaserCW(Hit.ImpactPoint, Bounces + 1);
		}
	}
	DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), false, 0.1f, 0, 3.f);
	return false;
}

bool ALightEmitter::SendLaserDown(FVector Start, int Bounces)
{
	if (Bounces > MaxBounces) return false;

	FVector UpVector = Start.GetSafeNormal();
	FVector End = Start - 1000.F * UpVector;

	FHitResult Hit;
	bool bHitSomething = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility);
	if (bHitSomething)
	{
		DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor(255, 0, 0), false, 0.1f, 0, 3.f);
		float HitPlaneAngle = FVector::DotProduct(Hit.ImpactPoint.GetSafeNormal(), Hit.Normal);

		UE_LOG(LogTemp, Warning, TEXT("Send Down hit something angle : %f"), HitPlaneAngle);
		if (HitPlaneAngle > -0.707F)
		{
			return SendLaserCCW(Hit.ImpactPoint, Bounces + 1);
		}
		else
		{
			return SendLaserCW(Hit.ImpactPoint, Bounces + 1);
		}
	}

	DrawDebugLine(GetWorld(), Start, End, FColor(255, 0, 0), false, 0.1f, 0, 3.f);
	return false;
}