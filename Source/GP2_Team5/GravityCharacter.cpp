// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityCharacter.h"
#include "GravityMovementComponent.h"

// Sets default values
AGravityCharacter::AGravityCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Tick is needed for gravity functionality
	PrimaryActorTick.bCanEverTick = true;
	CachedGravityMovementyCmp = Cast<UGravityMovementComponent>(GetMovementComponent());
}

bool AGravityCharacter::CanSwap(TScriptInterface<IGravitySwappable> Other)
{
	return true;
}

void AGravityCharacter::SwapGravity(TScriptInterface<IGravitySwappable> Other)
{
	bool bThisFlipGravity = GetFlipGravity();
	if (Other)
	{
		SetFlipGravity(Other->GetFlipGravity());
		Other->SetFlipGravity(bThisFlipGravity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Flip gravity with null on objet : %s"), *this->GetName());
		SetFlipGravity(!bThisFlipGravity);
	}
}

void AGravityCharacter::ClickInteract_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ClickInteract: %s"), *this->GetName());
}

void AGravityCharacter::ResetClickInteract_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ResetClickInteract: %s"), *this->GetName());
}

bool AGravityCharacter::GetFlipGravity()
{
	return bFlipGravity;
}

void AGravityCharacter::SetFlipGravity(bool bNewGravity)
{
	bFlipGravity = bNewGravity;
	OnGravityChanged.Broadcast(bNewGravity);
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

