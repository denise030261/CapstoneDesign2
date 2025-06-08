// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalismanSkillStrategy.h"
#include <Runtime/LevelSequence/Public/LevelSequence.h>
#include <Runtime/LevelSequence/Public/LevelSequencePlayer.h>
#include "GodSkill.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UGodSkill : public UTalismanSkillStrategy
{
	GENERATED_BODY()
	
public:
	virtual void SkillExecute_Implementation(ATalisman* Owner, UWorld* World) override;

	UPROPERTY(EditAnywhere)
	ULevelSequence* LevelSequence;

private:
	ULevelSequencePlayer* LevelSequencePlayer;

	UFUNCTION()
	void DoneSkill();

	bool bUsing = false;

	UFUNCTION()
	void AllCharacterMove(UWorld* World, bool bMove);

	UPROPERTY() 
	TWeakObjectPtr<UWorld> CachedWorld;
};
