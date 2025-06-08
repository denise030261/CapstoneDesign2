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

    UFUNCTION(BlueprintCallable)
    bool GetEverBossRoad() const { return EverBossRoad; }

    UFUNCTION(BlueprintCallable)
    void SetEverBossRoad(bool b) { EverBossRoad = b; }


    UPROPERTY(BlueprintReadWrite)
    bool EverTown;

    UFUNCTION(BlueprintCallable)
    bool GetEverTown() const { return EverTown; }

    UFUNCTION(BlueprintCallable)
    void SetEverTown(bool b) { EverTown = b; }

    UPROPERTY(BlueprintReadWrite)
    int32 MonsterCount;

    UPROPERTY(BlueprintReadWrite)
    FString QuestString;

    UPROPERTY(BlueprintReadWrite)
    FString RepresentativeQuestString;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    int32 IronCount = 0;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    bool GetNPC2Quest;

    UFUNCTION(BlueprintCallable)
    void AddIronCount() { IronCount++; }

    UFUNCTION(BlueprintCallable)
    int32 GetAddIronCount() const { return IronCount; }

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    bool IsPattern1;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    bool IsPattern2;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    bool IsPattern3;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    bool NoPattern;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool UnLockSkill;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    bool KillBossProcess;
};
