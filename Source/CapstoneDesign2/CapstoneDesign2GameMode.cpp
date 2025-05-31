// Copyright Epic Games, Inc. All Rights Reserved.

#include "CapstoneDesign2GameMode.h"
#include "System/MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"

AActor* ACapstoneDesign2GameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    FString PrevMap = GI ? GI->PreviousMapName : "";

    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

    for (AActor* Start : PlayerStarts)
    {
        if (APlayerStart* PS = Cast<APlayerStart>(Start))
        {
            if (PrevMap == "NewTuto" && PS->PlayerStartTag == "FromMapTuto")
                return PS;

            if (PrevMap == "NewBossRoad" && PS->PlayerStartTag == "FromMapBoss")
                return PS;
        }
    }

    return Super::ChoosePlayerStart_Implementation(Player);
}


ACapstoneDesign2GameMode::ACapstoneDesign2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Script/Engine.Blueprint'/Game/CapstoneDesign/Blueprints/BP_MainCharacter.BP_MainCharacter_C'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
