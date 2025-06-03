// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void UMyGameInstance::SetSavedUI(UUserWidget* InUI)
{
    SavedUI = InUI;
}

UUserWidget* UMyGameInstance::GetSavedUI() const
{
    return SavedUI;
}

