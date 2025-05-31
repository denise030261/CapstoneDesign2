// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MonsterUI.generated.h"

class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UMonsterUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	FString MonsterName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float NowHp;
	
	UFUNCTION(BlueprintCallable)
	void SetMonsterName(const FString Name);
	
	UFUNCTION(BlueprintCallable)
	void SetMaxHp(const float Hp);
	
	UFUNCTION(BlueprintCallable)
	void SetHp(const float Hp);

	UFUNCTION(BlueprintCallable)
	float GetHpPercentage() const;
};
