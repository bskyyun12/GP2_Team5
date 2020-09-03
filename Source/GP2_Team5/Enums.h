// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum FocusType
{
	Player	UMETA(DisplayName = "Player"),
	Object	UMETA(DisplayName = "Object"),
	Null	UMETA(DisplayName = "Null"),
};

class GP2_TEAM5_API Enums
{
public:
	Enums();
	~Enums();
};
