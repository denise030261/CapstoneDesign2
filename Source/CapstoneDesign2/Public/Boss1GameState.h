// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Runtime/LevelSequence/Public/LevelSequence.h>
#include <Runtime/LevelSequence/Public/LevelSequencePlayer.h>
#include "GameFramework/GameStateBase.h"
#include "Boss1GameState.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API ABoss1GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ULevelSequence* LevelSequence;

protected:
	virtual void BeginPlay() override;
	
private:
	ULevelSequencePlayer* LevelSequencePlayer;

	UFUNCTION()
	void DoneAppearScene();

	UFUNCTION()
	void AllCharacterMove(UWorld* World, bool bMove);
};
