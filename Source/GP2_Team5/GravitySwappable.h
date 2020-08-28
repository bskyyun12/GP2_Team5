// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GravitySwappable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGravityChanged, bool, bInverted);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGravitySwappable : public UInterface
{
	GENERATED_BODY()

};

/**
 * 
 */
class GP2_TEAM5_API IGravitySwappable
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintNativeEvent)
	bool GetFlipGravity();

	UFUNCTION(BlueprintNativeEvent)
	void SetFlipGravity(bool bNewGravity);

	////UFUNCTION(BlueprintCallable)
	////virtual bool CanSwap(TScriptInterface<IGravitySwappable> Other) = 0 ;

	////UFUNCTION(BlueprintCallable)
	////virtual void SwapGravity(TScriptInterface<IGravitySwappable> Other) = 0;

	////UFUNCTION(BlueprintCallable)
	////virtual bool GetFlipGravity() = 0;

	////UFUNCTION(BlueprintCallable)
	////virtual void SetFlipGravity(bool bNewGravity) = 0;
};