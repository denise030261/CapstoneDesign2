// Fill out your copyright notice in the Description page of Project Settings.


#include "GodSkill.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include <Boss1/Boss1.h>
#include <CapstoneDesign2/MainCharacter.h>
#include <Kismet/GameplayStatics.h>
#include "GameFramework/CharacterMovementComponent.h"

void UGodSkill::SkillExecute_Implementation(ATalisman* Owner, UWorld* World)
{
	ALevelSequenceActor* OutLevelSequenceActor = nullptr;  // LevelSequenceActor를 받을 변수 선언
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		World, LevelSequence, FMovieSceneSequencePlaybackSettings(), OutLevelSequenceActor);

	if (LevelSequencePlayer)
	{
		if (World)
		{
			AllCharacterMove(false);
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
	AllCharacterMove(true);
	UE_LOG(LogTemp, Warning, TEXT("Done Skill"));
}

void UGodSkill::AllCharacterMove(bool bMove)
{
	AMainCharacter* Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (Player && Player->GetCharacterMovement())
	{
		Player->GetCharacterMovement()->DisableMovement();
		Player->SetActorHiddenInGame(!bMove);
		Player->SetActorEnableCollision(bMove);
		Player->SetActorTickEnabled(bMove);
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss1::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		ABoss1* Boss = Cast<ABoss1>(FoundActors[0]);
		if (Boss)
		{
			Boss->SetActorHiddenInGame(!bMove);
			Boss->SetActorEnableCollision(bMove);
			Boss->SetActorTickEnabled(bMove);
		}
	}
}
