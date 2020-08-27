// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GravityCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GravitySwappable.h"
#include "ClickInteract.h"

#include "GravityCube.generated.h"

/**
 * 
 */
UCLASS()
class GP2_TEAM5_API AGravityCube : public AActor, public IGravitySwappable, public IClickInteract
{
	GENERATED_BODY()

	AGravityCube();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetGravityTarget(FVector NewGravityPoint);

public:

	// Delegates

	UPROPERTY(BlueprintAssignable)
	FOnGravityChanged OnGravityChanged;

	// IGravitySwappable
	virtual bool CanSwap(TScriptInterface<IGravitySwappable> Other) override;
	virtual void SwapGravity(TScriptInterface<IGravitySwappable> Other) override;
	bool GetFlipGravity() override;
	void SetFlipGravity(bool bNewGravity) override;

	// IClickInteract
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ClickInteract() override;
	virtual void ClickInteract_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ResetClickInteract() override;
	virtual void ResetClickInteract_Implementation();

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
