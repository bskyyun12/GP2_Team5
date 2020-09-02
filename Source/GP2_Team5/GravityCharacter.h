// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ApproachInteract.h"
#include "GravitySwappable.h"
#include "ClickInteract.h"
#include "Collectible.h"
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

	virtual void AddCollectible(ACollectible* Collectible);

protected: 
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void MoveRight(float Val);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Sets the point the character gravitates towards
	UFUNCTION(BlueprintCallable)
	void SetGravityTarget(FVector NewGravityPoint);

	UFUNCTION(BlueprintImplementableEvent, Category = "Collectible")
	void OnCollectibleAdded(ECollectibleType Type, int32 NewCount);

	UFUNCTION(BlueprintPure)
	UGravityMovementComponent* GetGravityMovementComponent() const { return CachedGravityMovementyCmp; }
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

// -- Member variables --
protected:
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "GravityCharacter|SaveData")
	TMap<ECollectibleType, int32> Collectibles;

	UGravityMovementComponent* CachedGravityMovementyCmp = nullptr;

	UPROPERTY(EditAnywhere, Category = "GravityCharacter|Gravity")
	FVector GravityPoint {};

	/* The rate at which gravity changes from old to new target */
	UPROPERTY(EditAnywhere, Category = "GravityCharacter|Gravity")
	float GravityChangeSpeed = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityCharacter|Gravity")
	bool bFlipGravity = false;

	UPROPERTY(EditDefaultsOnly, Category = "GravityCharacter|Camera")
	bool bRotateCameraToPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* SideViewCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

#pragma region Jump

	void Jump();

	UFUNCTION(BlueprintPure, Category = "GravityCharacter|Jump")
	bool IsJumping();

#pragma endregion


#pragma region Interaction

	UPROPERTY(EditAnywhere, Category = "GravityCharacter|Interaction")
	class UBoxComponent* InteractBox;

	UPROPERTY(EditAnywhere, Category = "GravityCharacter|Interaction")
	float ClickInteractRange = 700.f;

	// Approach Interact
	void OnApproachInteract();
	void OnApproachInteractReleased();
	UActorComponent* TryGetApproachInteractableComp();
	UActorComponent* ApproachInteractableComp = nullptr;

	// Click Interact
	void OnClickInteract();
	UActorComponent* CurrentClickFocus = nullptr;
	void ResetClickInteract(UActorComponent*& FocusToReset);
	bool CanSwapGravity(UActorComponent* Comp1, UActorComponent* Comp2);

	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityCharacter|Interaction")
	bool bIsGrabbing = false;

	// Powers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityCharacter|Interaction")
	bool bHasRelic1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityCharacter|Interaction")
	bool bHasRelic2 = false;

	/// Check list
	/// while grabbing -> NO: click interact / approach interact / jump
	/// while having first click focus -> NO: move, jump, approach interact -> reset first focus
	/// while jumping -> NO: grab / click interact / approach interact(?)
	/// 
	/// </summary>

#pragma endregion
};
