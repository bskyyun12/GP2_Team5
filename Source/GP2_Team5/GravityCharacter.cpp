// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityMovementComponent.h"
#include "GravityCharacter.h"

// Sets default values
AGravityCharacter::AGravityCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Tick is needed for gravity functionality
	PrimaryActorTick.bCanEverTick = true;
	CachedGravityMovementyCmp = Cast<UGravityMovementComponent>(GetMovementComponent());
}

// Called when the game starts or when spawned
void AGravityCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGravityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto oldGravDir = CachedGravityMovementyCmp->GetGravityDirection();
	auto newGravDir = GravityPoint - GetActorLocation();
	newGravDir.Normalize();
	if (bFlipGravity)
	{
		newGravDir = newGravDir * -1.f;
	}

	auto result = FMath::VInterpTo(oldGravDir, newGravDir, DeltaTime, GravityChangeSpeed);
	CachedGravityMovementyCmp->SetGravityDirection(result);
}

// Called to bind functionality to input
void AGravityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGravityCharacter::SetGravityTarget(FVector NewGravityPoint)
{
	GravityPoint = NewGravityPoint;
}

