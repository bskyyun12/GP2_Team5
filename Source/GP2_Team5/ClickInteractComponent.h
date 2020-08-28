// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClickInteract.h"
#include "ClickInteractComponent.generated.h"


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

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractDelegate OnClickInteract;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractDelegate OnResetClickInteract;

	void ClickInteract();
	virtual void ClickInteract_Implementation();

	void ResetClickInteract();
	virtual void ResetClickInteract_Implementation();

};