// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalismanSkillStrategy.h"
#include "MoveSkill.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UMoveSkill : public UTalismanSkillStrategy
{
	GENERATED_BODY()

public:
	virtual void SkillExecute_Implementation(ATalisman* Owner, UWorld* World) override;

	UFUNCTION()
	void Moving(FVector TargetLocation, ACharacter* Character);
	
};
