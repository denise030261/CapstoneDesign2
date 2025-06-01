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

	virtual void BeginPlay() override;

	// UI 저장 함수
	UFUNCTION(BlueprintCallable)
	void SetSavedUI(UUserWidget* InUI);

	// UI 꺼내는 함수
	UFUNCTION(BlueprintCallable)
	UUserWidget* GetSavedUI() const;

protected:
	// 위젯 클래스 선택 가능
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> UIClass;

	// 생성된 UI 저장 변수
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	UUserWidget* SavedUI;
};



