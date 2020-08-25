// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Components/BoxComponent.h>
#include "InteractionInterface.h"
#include "GP2_Team5Character.generated.h"

class UGravityMovementComponent;

UCLASS(config = Game)
class AGP2_Team5Character : public ACharacter
{
	GENERATED_BODY()

public:
	//AGP2_Team5Character();
	AGP2_Team5Character(const FObjectInitializer& ObjectInitializer);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// Gravity stuff
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	UGravityMovementComponent* GetGravityMovementComponent();

public:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* InteractBox;

	IInteractionInterface* Interactable = nullptr;

	void OnInteract();
};

