// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ApproachInteract.h"
#include "ApproachInteractComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP2_TEAM5_API UApproachInteractComponent : public UActorComponent, public IApproachInteract
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UApproachInteractComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractDelegate);
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractDelegate OnInteract;

	void Interact();
	virtual void Interact_Implementation();

	void InteractReleased();
	virtual void InteractReleased_Implementation();

	void ShowInteractionWidget() override;
	void HideInteractionWidget() override;
		
};
