// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Boss1UI.generated.h"

class ABoss1_Base;
/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UBoss1UI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	ABoss1_Base* Boss1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float HpPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	TArray<bool> IsEatIron = { false, false, false };
};
