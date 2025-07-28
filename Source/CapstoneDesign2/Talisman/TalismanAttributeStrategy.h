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
    UFUNCTION()
    virtual void Attack_Implementation(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman) PURE_VIRTUAL(UTalismanAttributeStrategy::Attack, );

    UFUNCTION()
    virtual void Passive_Implementation(UWorld* World) PURE_VIRTUAL(UTalismanAttributeStrategy::Passive, );

    UFUNCTION(BlueprintCallable, Category = "Attribute")
    void PlayHitSound(UWorld* World, FVector Location);

protected:
    UPROPERTY(EditAnywhere, Category = "Talisman")
    USoundBase* HitSound;
};