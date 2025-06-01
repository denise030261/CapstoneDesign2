// Fill out your copyright notice in the Description page of Project Settings.


#include "GodSkill.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

void UGodSkill::SkillExecute_Implementation(ATalisman* Owner, UWorld* World)
{
	ALevelSequenceActor* OutLevelSequenceActor = nullptr;  // LevelSequenceActor를 받을 변수 선언
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		World, LevelSequence, FMovieSceneSequencePlaybackSettings(), OutLevelSequenceActor);

	if (LevelSequencePlayer)
	{
		if (World)
		{
			// 무력화
		}
		LevelSequencePlayer->OnFinished.AddDynamic(this, &UGodSkill::DoneSkill); //=> 끝낼 때 Input 활성화
		LevelSequencePlayer->Play();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to create level sequence player"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Activated"));

}

void UGodSkill::DoneSkill()
{
	// Player, Boss Enable
	UE_LOG(LogTemp, Warning, TEXT("Done Skill"));
}
