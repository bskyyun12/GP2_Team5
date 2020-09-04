// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ApproachInteract.h"
#include "GravitySwappable.h"
#include "ClickInteract.h"
#include "ClickInteractComponent.h"
#include "Collectible.h"
#include "Enums.h"
#include "GravityCharacter.generated.h"

template<class T>
UActorComponent* GetComponentByInterface(AActor* Actor) 
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

template<class T>
TArray<T*> SphereOverlapComponents(UObject* World, FVector SphereCenter, float SphereRadius)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_MAX));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> OutActors;
	TArray<T*> OverlapingComponents;
	UKismetSystemLibrary::SphereOverlapActors(World, SphereCenter, SphereRadius, ObjectTypes, AActor::StaticClass(), ActorsToIgnore, OutActors);
	for (AActor* OverlapingActor : OutActors)
	{
		UActorComponent* Comp = OverlapingActor->GetComponentByClass(T::StaticClass());
		if (Comp != nullptr)
		{
			OverlapingComponents.Add(Cast<T>(Comp));
		}
	}
	return OverlapingComponents;
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
protected:

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
	UClickInteractComponent* CurrentClickFocus = nullptr;
	void ResetClickInteract(UClickInteractComponent*& FocusToReset);

	// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityCharacter|Interaction")
	bool bIsGrabbing = false;

	// Powers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityCharacter|Interaction")
	bool bHasRelic1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GravityCharacter|Interaction")
	bool bHasRelic2 = false;

public:
	bool CanSwapGravity(UActorComponent* Comp1, UActorComponent* Comp2);
	float GetClickInteractRange() { return ClickInteractRange; }
	UClickInteractComponent* GetCurrentClickFocus() { return CurrentClickFocus; }
	EFocusType GetClickFocusType(UClickInteractComponent* ClickFocus);
	bool IsComponentInLineOfSight(UActorComponent* Comp);

	UFUNCTION(BlueprintPure)
	bool HasCurrentClickFocus() { return CurrentClickFocus != nullptr; }

	UFUNCTION(BlueprintPure)
	bool HasRelic1() { return bHasRelic1; }

	UFUNCTION(BlueprintPure)
	bool HasRelic2() { return bHasRelic2; }
	/// Check list
	/// while grabbing -> NO: click interact / approach interact / jump
	/// while having first click focus -> NO: move, jump, approach interact -> reset first focus
	/// while jumping -> NO: grab / click interact / approach interact(?)
	/// 
	/// </summary>


#pragma endregion
};
