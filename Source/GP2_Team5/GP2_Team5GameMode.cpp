// Copyright Epic Games, Inc. All Rights Reserved.

#include "GP2_Team5GameMode.h"
#include "GP2_Team5Character.h"
#include "UObject/ConstructorHelpers.h"

AGP2_Team5GameMode::AGP2_Team5GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
