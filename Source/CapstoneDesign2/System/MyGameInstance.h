// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "MyGameInstance.generated.h"



UCLASS()
class CAPSTONEDESIGN2_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    FString PreviousMapName;

    UPROPERTY(BlueprintReadWrite)
    bool EverBossRoad;

    UPROPERTY(BlueprintReadWrite)
    bool EverTown;

    UPROPERTY(BlueprintReadWrite)
    int32 MonsterCount;

    UPROPERTY(BlueprintReadWrite)
    FString QuestString;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    int32 IronCount = 0;

    UFUNCTION(BlueprintCallable)
    void AddIronCount() { IronCount++; }

    UFUNCTION(BlueprintCallable)
    int32 GetAddIronCount() const { return IronCount; }
};
