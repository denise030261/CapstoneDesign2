// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CanvasManager.generated.h"

UCLASS()
class CAPSTONEDESIGN2_API ACanvasManager : public AActor
{
	GENERATED_BODY()
	
public:
    ACanvasManager();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
    TArray<AActor*> CanvasActors;  // Blueprint에서 캔버스 액터 직접 연결

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
    int32 CurrentAnswerIndex = 0;

    UFUNCTION(BlueprintCallable, Category = "Canvas")
    bool CheckAnswer();


    UFUNCTION(BlueprintCallable)
    bool CheckAllAnswersAndReset();

private:
    TArray<TArray<bool>> AnswerPatterns;

    void InitAnswerPatterns();


};