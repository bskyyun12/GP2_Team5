// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GravityCharacter.h"
#include "Components/BoxComponent.h"
#include "InteractionInterface.h"
#include "GravityCube.h"
#include "GravityPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GP2_TEAM5_API AGravityPlayerCharacter : public AGravityCharacter
{
	GENERATED_BODY()

public:
	AGravityPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }


protected:

	virtual void Tick(float DeltaTime) override;

	/** Called for side to side input */
	void MoveRight(float Val);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		bool bUseCameraRotation = false;

private:
	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	bool bRotateCameraToPlayer = false;

	///////////////////////////////////////
	// Interaction
	UPROPERTY(EditAnywhere)
	UBoxComponent* InteractBox;

	IInteractionInterface* Interactable = nullptr;

	void OnInteract();
	
	IInteractionInterface* GetClosestInteracterbleActor();

	void OnSelect();

	UPROPERTY(VisibleAnywhere)
	AGravityCube* CurrentFocus;
	// Interaction
	///////////////////////////////////////
};
