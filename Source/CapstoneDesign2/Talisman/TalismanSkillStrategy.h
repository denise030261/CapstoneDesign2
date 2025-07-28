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
    UFUNCTION()
    virtual void SkillExecute_Implementation(ATalisman* Owner, UWorld* World) PURE_VIRTUAL(UTalismanSkillStrategy::SkillExecute, );
    // As soon as Attacked

    UFUNCTION()
    virtual void HitExecute_Implementation(UWorld* World, AActor* OtherActor, ATalisman* Talisman) PURE_VIRTUAL(UTalismanSkillStrategy::HitExecute, );
    // As soon as Hited
};
