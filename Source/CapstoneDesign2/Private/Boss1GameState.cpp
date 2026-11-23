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
    Super::BeginPlay();

    FTimerHandle InitTimerHandle;
    float DelayInSeconds = 0.2f; 
    GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ABoss1GameState::PlayAppearScene, DelayInSeconds, false);
}

void ABoss1GameState::CatchBoss()
{
    if (bDie)
        return;

    bDie = true;

    UWorld* World = GetWorld();
    if (World == nullptr)
        return;

    ALevelSequenceActor* OutLevelSequenceActor = nullptr;  
    LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
        World, EndLevelSequence, FMovieSceneSequencePlaybackSettings(), OutLevelSequenceActor);

    if (LevelSequencePlayer)
    {
        if (World)
        {
            AllCharacterMove(World, false);
        }
        LevelSequencePlayer->OnFinished.AddDynamic(this, &ABoss1GameState::DoneBossScene);
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

void ABoss1GameState::DoneBossScene()
{
    AllCharacterMove(GetWorld(), true);

    for (int i = 0; i < Spawns.Num(); i++)
    {
        FActorSpawnParameters SpawnParams;
        ACharacter* SpawnedActor = GetWorld()->SpawnActor<ACharacter>(
            Spawns[i],
            SpawnVectors[i],
            SpawnRotators[i],
            SpawnParams);
    }
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
        for (int i = 0; i < FoundActors.Num(); i++)
        {
            ABoss1* Boss = Cast<ABoss1>(FoundActors[i]);
            if (Boss)
            {
                Boss->SetActorHiddenInGame(!bMove);
                Boss->SetActorTickEnabled(bMove);
                Boss->FootstepSoundComp->Stop();
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not Boss"));
    }
}

bool ABoss1GameState::PrepareAppearSceneForLoading()
{
    if (bAppearScenePrepared)
        return IsValid(LevelSequencePlayer);
    UWorld* World = GetWorld();
    if (!World || !LevelSequence)
        return false;
    ALevelSequenceActor* SequenceActor = nullptr;
    LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
        World, LevelSequence, FMovieSceneSequencePlaybackSettings(), SequenceActor);
    if (!LevelSequencePlayer)
        return false;
    AllCharacterMove(World, false);
    // AppearScene has no event tracks. Evaluate the opening pose while stopped;
    // do not advance gameplay timers or start the cinematic audio.
    LevelSequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams(
        LevelSequencePlayer->GetStartTime().Time, EUpdatePositionMethod::Jump));
    bAppearScenePrepared = true;
    UE_LOG(LogTemp, Display, TEXT("Boss intro opening frame prepared at world time %.3f"), World->GetTimeSeconds());
    return true;
}

void ABoss1GameState::PlayAppearScene()
{
    if (bAppearSceneStarted || !PrepareAppearSceneForLoading())
        return;
    bAppearSceneStarted = true;
    LevelSequencePlayer->OnFinished.AddDynamic(this, &ABoss1GameState::DoneAppearScene);
    UE_LOG(LogTemp, Display, TEXT("Boss intro playback started at world time %.3f"), GetWorld()->GetTimeSeconds());
    LevelSequencePlayer->Play();
}
