// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFocusType : uint8
{
	Player	UMETA(DisplayName = "Player"),
	Object	UMETA(DisplayName = "Object"),
	Null	UMETA(DisplayName = "Null"),
};

UENUM(BlueprintType)
enum class ECollectibleType : uint8
{
	Coin,
	Relic1,
	Relic2,
};

UENUM(BlueprintType)
enum class EHighlightType : uint8
{
	Hover,
	Swappable,
	NonClickable,
};

class GP2_TEAM5_API Enums
{
public:
	Enums();
	~Enums();
};
