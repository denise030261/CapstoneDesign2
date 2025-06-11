// Copyright Epic Games, Inc. All Rights Reserved.

#include "CapstoneDesign2GameMode.h"
#include "System/MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerStart.h"

AActor* ACapstoneDesign2GameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
    FName DesiredTag = NAME_None;

    if (GI)
    {
        if (GI->PreviousMapName == "NewTuto")
        {
            DesiredTag = FName("FromTuto");
        }
        else if (GI->PreviousMapName == "NewBossRoad")
        {
            DesiredTag = FName("FromBoss");
        }
    }

    // �±׿� �´� PlayerStart ã��
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

    for (AActor* Start : PlayerStarts)
    {
        if (APlayerStart* PS = Cast<APlayerStart>(Start))
        {
            if (PS->PlayerStartTag == DesiredTag)
            {
                UE_LOG(LogTemp, Warning, TEXT("%s"), *PS->GetName());
                return PS;
            }
        }
    }

    // �� ã���� �⺻ ��ġ
    return Super::ChoosePlayerStart_Implementation(Player);
}



ACapstoneDesign2GameMode::ACapstoneDesign2GameMode()
{
}


