// Copyright Epic Games, Inc. All Rights Reserved.

#include "CapstoneDesign2GameMode.h"
#include "CapstoneDesign2Character.h"
#include "UObject/ConstructorHelpers.h"

ACapstoneDesign2GameMode::ACapstoneDesign2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
