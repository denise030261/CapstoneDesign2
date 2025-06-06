// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "CapstoneDesign2GameMode.generated.h"

UCLASS(Blueprintable)
class ACapstoneDesign2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACapstoneDesign2GameMode();

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;


};



