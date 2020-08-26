// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GravityCharacter.h"
#include "Components/StaticMeshComponent.h"

#include "GravityCube.generated.h"

/**
 * 
 */
UCLASS()
class GP2_TEAM5_API AGravityCube : public AActor
{
	GENERATED_BODY()

	AGravityCube();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetGravityTarget(FVector NewGravityPoint);


protected:

	UPROPERTY(EditAnywhere, Category = "Gravity")
	FVector GravityPoint {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bFlipGravity = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mesh")
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity");
	float GravityAcceleration = 500.F;

private:
	
};
