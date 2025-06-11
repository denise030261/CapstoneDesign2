// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDieWidget.h"
#include <Kismet/GameplayStatics.h>

void UPlayerDieWidget::NativeConstruct()
{
	//ButtonCanvas->SetVisibility(ESlateVisibility::Hidden);
	//DieText->SetText(FText::FromString(""));
	FLinearColor CurrentColor = BackgroundImage->GetColorAndOpacity();
	CurrentColor.A = 1.0f;
	BackgroundImage->SetColorAndOpacity(CurrentColor);
	bTurnOnWidget = false;
	CountTime = 0;
	CurrentIndex = 0;

	RetryButton->OnClicked.AddDynamic(this, &UPlayerDieWidget::GoToBossLevel);
	GotoButton->OnClicked.AddDynamic(this, &UPlayerDieWidget::GoToMainMenu);
}

void UPlayerDieWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	FLinearColor CurrentColor = BackgroundImage->GetColorAndOpacity();
	CurrentColor.A += InDeltaTime;
	if(!bTurnOnWidget)
	{
		if (CurrentColor.A >= 0.95)
		{
			CurrentColor.A = 1;
			BackgroundImage->SetColorAndOpacity(CurrentColor);
			bTurnOnWidget = true;
		}
		else
		{
			BackgroundImage->SetColorAndOpacity(CurrentColor);
		}
	}
	else if (bTurnOnWidget && CountTime>= TextTime)
	{
		CountTime = 0;
		FString CurrentText = DieText->GetText().ToString();
		if (CurrentIndex >= DieString.Len())
		{
			ButtonCanvas->SetVisibility(ESlateVisibility::Visible);
			bTextDone = true;
			return;
		}
		CurrentText += DieString[CurrentIndex];
		DieText->SetText(FText::FromString(CurrentText));
	}
	else if(!bTextDone)
	{
		CountTime += InDeltaTime;
	}
}

void UPlayerDieWidget::GoToBossLevel()
{
	FInputModeGameOnly InputModeData;
	GetWorld()->GetFirstPlayerController()->SetInputMode(InputModeData);
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	UGameplayStatics::OpenLevel(this, FName("NewBoss"));
}

void UPlayerDieWidget::GoToMainMenu()
{
	UGameplayStatics::OpenLevel(this, FName("StartMap"));
}
