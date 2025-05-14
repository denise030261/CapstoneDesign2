// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TalismanAttributeStrategy.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class CAPSTONEDESIGN2_API UTalismanAttributeStrategy : public UObject
{
    GENERATED_BODY()

public:
    // 공격 함수 (자식 클래스에서 구현)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attribute")
    void Attack(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman);
    virtual void Attack_Implementation(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman) PURE_VIRTUAL(UTalismanAttributeStrategy::Attack, );

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attribute")
    void Passive(UWorld* World);
    virtual void Passive_Implementation(UWorld* World) PURE_VIRTUAL(UTalismanAttributeStrategy::Passive, );
};