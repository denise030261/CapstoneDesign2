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

	UPROPERTY(EditAnywhere)
	ULevelSequence* EndLevelSequence;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ACharacter>> Spawns;

	UPROPERTY(EditAnywhere)
	TArray<FVector> SpawnVectors;

	UPROPERTY(EditAnywhere)
	TArray<FRotator> SpawnRotators;

	UFUNCTION()
	void CatchBoss();

	bool PrepareAppearSceneForLoading();

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(Transient)
	ULevelSequencePlayer* LevelSequencePlayer;
	bool bAppearScenePrepared = false;
	bool bAppearSceneStarted = false;

	UFUNCTION()
	void DoneAppearScene();

	UFUNCTION()
	void DoneBossScene();

	UFUNCTION()
	void AllCharacterMove(UWorld* World, bool bMove);

	bool bDie = false;

	void PlayAppearScene();
};
