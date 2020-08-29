// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ApproachInteract.h"
#include "GravitySwappable.h"
#include "ClickInteract.h"
#include "GravityCharacter.generated.h"


template<class T>
UActorComponent* GetComponent(AActor* Actor) 
{	
	TArray<UActorComponent*> Components = Actor->GetComponentsByInterface(T::StaticClass());
	for (UActorComponent* Comp : Components)
	{
		if (Comp->GetClass()->ImplementsInterface(T::StaticClass()))
		{
			return Comp;
		}
	}
	return nullptr;
}

//--- forward declarations ---
class UGravityMovementComponent;

UCLASS()
class GP2_TEAM5_API AGravityCharacter : public ACharacter, public IGravitySwappable
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AGravityCharacter(const FObjectInitializer& ObjectInitializer);

	// IGravitySwappable
	virtual bool GetFlipGravity() const override;
	virtual void SetFlipGravity(bool bNewGravity) override;


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

#pragma region Interaction

	UPROPERTY(EditAnywhere, Category = "Interaction")
	class UBoxComponent* InteractBox;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float ClickInteractRange = 700.f;

	// Approach Interact
	void OnInteract();
	void OnInteractReleased();
	UActorComponent* TryGetApproachInteractableComp();
	UActorComponent* ApproachInteractableComp = nullptr;

	// Click Interact
	void OnClick();
	UActorComponent* CurrentClickFocus = nullptr;
	void ResetClickInteract(UActorComponent*& FocusToReset);

	// 
	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bIsGrabbing = false;

	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool HasCurrentFocus() {	return CurrentClickFocus != nullptr;	}

	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool IsJumping();


	/// Check list
	/// while grabbing -> NO: click interact / approach interact / jump
	/// while having first click focus -> NO: move, jump, approach interact -> reset first focus
	/// while jumping -> NO: grab / click interact / approach interact(?)
	/// 
	/// </summary>

#pragma endregion

};