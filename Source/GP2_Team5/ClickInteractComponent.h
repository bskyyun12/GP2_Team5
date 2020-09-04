// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClickInteract.h"
#include "ClickInteractComponent.generated.h"


template<class T>
UMeshComponent* GetMeshComponent(AActor* Actor)
{
	TArray<T*> Components;
	Actor->GetComponents<T>(Components);
	for (int32 i = 0; i < Components.Num(); i++)
	{
		return Components[i];
	}

	return nullptr;
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP2_TEAM5_API UClickInteractComponent : public UActorComponent, public IClickInteract
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClickInteractComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightActivationDelegate, bool, bIsWithinRange, FColor, HightlightColor);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightDeactivationDelegate, bool, bIsWithinRange, bool, bClickable);


	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractDelegate OnClickInteract;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractDelegate OnResetClickInteract;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FHighlightActivationDelegate OnActivateHighlight;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FHighlightDeactivationDelegate OnDeactivateHighlight;

	bool bSelected = false;
	bool bIsSwappableColorOn = false;

	UPROPERTY(EditAnywhere)
	FColor HoverColor = FColor::Green;
	UPROPERTY(EditAnywhere)
	FColor SwappableColor = FColor::Cyan;
	UPROPERTY(EditAnywhere)
	FColor NonClickableColor = FColor::Red;


public:

	bool Clickable();
	class AGravityCharacter* GetGravityCharacter();
	void OnReset();

	UFUNCTION()
	void ActivateHighlight(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void SwappableHighlight();
	UFUNCTION()
	void DeactivateHighlight(UPrimitiveComponent* TouchedComponent);

	void ClickInteract();
	virtual void ClickInteract_Implementation();

	void ResetClickInteract();
	virtual void ResetClickInteract_Implementation();

};