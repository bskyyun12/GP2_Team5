// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InteractionInterface.h"
#include "GravitySwappable.h"
#include "ClickInteract.h"
#include "GravityCharacter.generated.h"



//--- forward declarations ---
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
	bool GetFlipGravity() override { return bFlipGravity; };
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
	virtual void Tick(float DeltaTime) override;
	void MoveRight(float Val);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Sets the point the character gravitates towards
	UFUNCTION(BlueprintCallable)
	void SetGravityTarget(FVector NewGravityPoint);

	UFUNCTION(BlueprintPure)
	UGravityMovementComponent* GetGravityMovementComponent() const { return CachedGravityMovementyCmp; }
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }



// -- Member variables --
protected:
	UGravityMovementComponent* CachedGravityMovementyCmp = nullptr;

	UPROPERTY(EditAnywhere, Category = "Gravity")
	FVector GravityPoint {};

	/* The rate at which gravity changes from old to new target */
	UPROPERTY(EditAnywhere, Category = "Gravity")
	float GravityChangeSpeed = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bFlipGravity = false;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	bool bRotateCameraToPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* SideViewCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

///////////////////////////////////////
// Interaction
	UPROPERTY(EditAnywhere)
	class UBoxComponent* InteractBox;

	UPROPERTY(EditAnywhere)
	float ClickInteractRange = 700.f;

	IInteractionInterface* Interactable = nullptr;

	TScriptInterface<IGravitySwappable> FirstFocus = nullptr;

	IClickInteract* CurrentClickFocus = nullptr;

	void OnInteract();

	IInteractionInterface* GetClosestInteracterbleActor();

	void OnClick();


// Interaction
///////////////////////////////////////
};
