// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1GameState.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include <Boss1/Boss1.h>
#include <CapstoneDesign2/MainCharacter.h>
#include <Kismet/GameplayStatics.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AudioComponent.h"

void ABoss1GameState::BeginPlay()
{
	UWorld* World = GetWorld();
	if (World==nullptr)
		return;

	ALevelSequenceActor* OutLevelSequenceActor = nullptr;  // LevelSequenceActor를 받을 변수 선언
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		World, LevelSequence, FMovieSceneSequencePlaybackSettings(), OutLevelSequenceActor);

	if (LevelSequencePlayer)
	{
		if (World)
		{
			AllCharacterMove(World, false);
		}
		LevelSequencePlayer->OnFinished.AddDynamic(this, &ABoss1GameState::DoneAppearScene);
		LevelSequencePlayer->Play();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to create level sequence player"));
	}
}

void ABoss1GameState::DoneAppearScene()
{
    AllCharacterMove(GetWorld(), true);
}

void ABoss1GameState::AllCharacterMove(UWorld* World, bool bMove)
{
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("AllCharacterMove: World is null!"));
        return;
    }

    AMainCharacter* Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (Player && Player->GetCharacterMovement())
    {
        if (bMove)
        {
            Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking); // 움직임 활성화
        }
        else
        {
            Player->GetCharacterMovement()->DisableMovement(); // 움직임 비활성화
        }
        Player->SetActorHiddenInGame(!bMove);
        Player->SetActorEnableCollision(bMove);
        Player->SetActorTickEnabled(bMove);
    }

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, ABoss1::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        ABoss1* Boss = Cast<ABoss1>(FoundActors[0]);
        if (Boss)
        {
            Boss->SetActorHiddenInGame(!bMove);
            Boss->SetActorTickEnabled(bMove);
            Boss->FootstepSoundComp->Stop();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not Boss"));
    }
}
