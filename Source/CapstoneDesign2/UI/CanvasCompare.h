// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CanvasCompare.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UCanvasCompare : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 유저가 그린 그림, 정답 패턴, 허용 오차
	UFUNCTION(BlueprintCallable, Category = "Canvas")

	static bool CompareCanvasToTarget(UTextureRenderTarget2D* UserCanvas, UTexture2D* TargetPattern, float Tolerance = 0.3f);
	
};
