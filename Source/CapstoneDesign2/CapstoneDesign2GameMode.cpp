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
}

void ACapstoneDesign2GameMode::BeginPlay()
{
    Super::BeginPlay();

    if (UIClass)
    {
        UUserWidget* CreatedUI = CreateWidget<UUserWidget>(GetWorld(), UIClass);
        if (CreatedUI)
        {
            CreatedUI->AddToViewport();
            CreatedUI->SetVisibility(ESlateVisibility::Visible);
            SavedUI = CreatedUI;
        }
    }
}

void ACapstoneDesign2GameMode::SetSavedUI(UUserWidget* InUI)
{
    SavedUI = InUI;
}

UUserWidget* ACapstoneDesign2GameMode::GetSavedUI() const
{
    return SavedUI;
}
