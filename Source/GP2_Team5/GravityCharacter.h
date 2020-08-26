// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GravitySwappable.h"
#include "GravityCharacter.generated.h"

// -- forward declarations --
class UGravityMovementComponent;


UCLASS()
class GP2_TEAM5_API AGravityCharacter : public ACharacter, public IGravitySwappable
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AGravityCharacter(const FObjectInitializer& ObjectInitializer);


	void CanSwap(IGravitySwappable* other) override;


	void FlipGravity() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Sets the point the character gravitates towards
	UFUNCTION(BlueprintCallable)
	void SetGravityTarget(FVector NewGravityPoint);

	UFUNCTION(BlueprintPure)
	UGravityMovementComponent* GetGravityMovementComponent() const { return CachedGravityMovementyCmp; }
protected:
	UGravityMovementComponent* CachedGravityMovementyCmp = nullptr;

	UPROPERTY(EditAnywhere, Category = "Gravity")
	FVector GravityPoint {};

	/* The rate at which gravity changes from old to new target */
	UPROPERTY(EditAnywhere, Category = "Gravity")
	float GravityChangeSpeed = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bFlipGravity = false;
};
