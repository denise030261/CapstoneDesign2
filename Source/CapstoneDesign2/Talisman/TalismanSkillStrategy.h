// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TalismanSkillStrategy.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class CAPSTONEDESIGN2_API UTalismanSkillStrategy : public UObject
{
	GENERATED_BODY()

public:
    // 스킬 함수 (자식 클래스에서 구현)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attribute")
    void SkillExecute(ATalisman* Owner, UWorld* World);
    virtual void SkillExecute_Implementation(ATalisman* Owner, UWorld* World) PURE_VIRTUAL(UTalismanSkillStrategy::SkillExecute, );
};
