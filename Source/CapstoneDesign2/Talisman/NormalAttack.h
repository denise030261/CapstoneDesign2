// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalismanSkillStrategy.h"
#include "NormalAttack.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UNormalAttack : public UTalismanSkillStrategy
{
	GENERATED_BODY()
	
public:
	virtual void SkillExecute_Implementation(ATalisman* Owner) override;

private:
	ATalisman* OwnTalisman;
};
