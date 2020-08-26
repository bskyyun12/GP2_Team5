// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityCube.h"
#include "Kismet/KismetMathLibrary.h"

AGravityCube::AGravityCube()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(false);
	Mesh->SetAngularDamping(10.f);
	Mesh->SetConstraintMode(EDOFMode::YZPlane);
	Mesh->SetCollisionProfileName("PhysicsActor");

	RootComponent = Mesh;
}

void AGravityCube::Tick(float DeltaTime)
{
	auto ClampedDeltaTime = FMath::Min(DeltaTime, 0.05f);
	auto GravityDirection = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), GravityPoint);

	auto force = ClampedDeltaTime * GravityAcceleration * Mesh->GetMass();
	auto forceVector = GravityDirection * force;
	//printvector_time(forceVector, DeltaTime);
	Mesh->AddForce(GravityDirection * force, NAME_None, true);
}

void AGravityCube::SetGravityTarget(FVector NewGravityPoint)
{
	GravityPoint = NewGravityPoint;
}
