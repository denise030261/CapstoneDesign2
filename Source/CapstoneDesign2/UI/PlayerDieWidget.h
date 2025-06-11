// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/TextBlock.h>
#include <Components/Image.h>
#include <Components/CanvasPanel.h>
#include <Components/Button.h>
#include "PlayerDieWidget.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UPlayerDieWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DieText;

	UPROPERTY(meta = (BindWidget))
	class UButton* RetryButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* GotoButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* ButtonCanvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FString DieString;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	float TextTime = 0.5f;

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION()
	void GoToBossLevel();

	UFUNCTION()
	void GoToMainMenu();

	bool bTurnOnWidget = false;

	bool bTextDone = false;

	float CountTime = 0;

	int CurrentIndex = 0;
};
