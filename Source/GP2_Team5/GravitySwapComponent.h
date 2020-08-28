// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravitySwappable.h"
#include "GravitySwapComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP2_TEAM5_API UGravitySwapComponent : public UActorComponent, public IGravitySwappable
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGravitySwapComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGravityChanged, bool, bInverted);
	UPROPERTY(BlueprintAssignable, Category = "Gravity")
	FOnGravityChanged OnFlipGravity;

	virtual bool GetFlipGravity() const override;
	virtual void SetFlipGravity(bool bNewGravity) override;

protected:

	UPROPERTY(EditAnywhere, Category = "Gravity")
	FVector GravityPoint {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bFlipGravity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bCanEverTick = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity");
	float GravityAcceleration = 1400.F;
};
