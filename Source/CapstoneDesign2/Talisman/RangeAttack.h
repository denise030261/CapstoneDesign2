// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalismanSkillStrategy.h"
#include "Containers/Queue.h"
#include "RangeAttack.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API URangeAttack : public UTalismanSkillStrategy
{
	GENERATED_BODY()
	
public:
	virtual void SkillExecute_Implementation(ATalisman* Owner, UWorld* World) override;

	void BombAttack(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman);

	void DuplicateAttack(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman);

private:
	ATalisman* OwnTalisman;

	AActor* TargetActor;

	UFUNCTION()
	void Bomb(ATalisman* Talisman, AActor* Target);

	TQueue<ATalisman*> DestoryTalismans;

	UWorld* CurrentWorld;
};
