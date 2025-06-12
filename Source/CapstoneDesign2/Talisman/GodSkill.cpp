// Fill out your copyright notice in the Description page of Project Settings.


#include "GodSkill.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include <Boss1/Boss1.h>
#include <CapstoneDesign2/MainCharacter.h>
#include <Kismet/GameplayStatics.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AudioComponent.h"

void UGodSkill::SkillExecute_Implementation(ATalisman* Owner, UWorld* World)
{
    CachedWorld = World;

	ALevelSequenceActor* OutLevelSequenceActor = nullptr;  // LevelSequenceActor를 받을 변수 선언
	LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		World, LevelSequence, FMovieSceneSequencePlaybackSettings(), OutLevelSequenceActor);

	if (LevelSequencePlayer)
	{
		if (World)
		{
			AllCharacterMove(World, false);
		}
		LevelSequencePlayer->OnFinished.AddDynamic(this, &UGodSkill::DoneSkill); 
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
    if (CachedWorld.IsValid())
    {
        UWorld* World = CachedWorld.Get();
        AllCharacterMove(World, true);
        UE_LOG(LogTemp, Warning, TEXT("Done Skill"));
    }
}

void UGodSkill::AllCharacterMove(UWorld* World, bool bMove)
{
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("AllCharacterMove: World is null!"));
        return;
    }

    AMainCharacter* Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (Player && Player->GetCharacterMovement())
    {
        UE_LOG(LogTemp, Warning, TEXT("Player"));
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
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not player"));
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
                UE_LOG(LogTemp, Warning, TEXT("Boss"));
                Boss->SetActorHiddenInGame(!bMove);
                Boss->SetActorEnableCollision(bMove);
                Boss->SetActorTickEnabled(bMove);
                Boss->FootstepSoundComp->Stop();

                IDamageable* InterfaceRef = Cast<IDamageable>(Boss);
                if (InterfaceRef && bMove)
                {
                    InterfaceRef->DealDamage(TalismanDataAsset->SkillInfo.Damage, TalismanDataAsset);
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not Boss"));
    }
}
