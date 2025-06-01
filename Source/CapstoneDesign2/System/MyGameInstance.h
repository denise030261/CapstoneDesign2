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
	// 이전 맵 이름을 저장
	FString PreviousMapName;
	
public:
    void SetSavedUI(UUserWidget* InUI);
    UUserWidget* GetSavedUI() const;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> UIClass;

private:
    UPROPERTY()
    UUserWidget* SavedUI;
};
