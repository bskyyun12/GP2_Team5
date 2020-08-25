// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include <Components/WidgetComponent.h>
#include "InteractableBase.generated.h"

UCLASS()
class GP2_TEAM5_API AInteractableBase : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void Interact();
	virtual void Interact_Implementation();

	virtual void ShowInteractionWidget() override;
	virtual void HideInteractionWidget() override;


private:
	UPROPERTY(EditAnywhere)
	USceneComponent* _RootComponent;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* InteractionWidget;

};
