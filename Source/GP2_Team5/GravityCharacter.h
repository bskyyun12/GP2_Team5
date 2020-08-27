// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GravitySwappable.h"
#include "ClickInteract.h"
#include "GravityCharacter.generated.h"



// -- forward declarations --
class UGravityMovementComponent;


UCLASS()
class GP2_TEAM5_API AGravityCharacter : public ACharacter, public IGravitySwappable, public IClickInteract
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AGravityCharacter(const FObjectInitializer& ObjectInitializer);

	// Delegates
	UPROPERTY(BlueprintAssignable)
	FOnGravityChanged OnGravityChanged;

	// IGravitySwappable
	bool CanSwap(TScriptInterface<IGravitySwappable> Other) override;
	void SwapGravity(TScriptInterface<IGravitySwappable> Other) override;
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
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
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
