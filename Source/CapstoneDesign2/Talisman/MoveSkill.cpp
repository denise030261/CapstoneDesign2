// Fill out your copyright notice in the Description page of Project Settings.

#include "MoveSkill.h"
#include "Talisman.h"
#include "GameFramework/Character.h"

void UMoveSkill::SkillExecute_Implementation(ATalisman* Owner, UWorld* World)
{
	Owner->SetActorTickEnabled(true);
	UE_LOG(LogTemp, Warning, TEXT("Move Skill"));
}

void UMoveSkill::Moving(FVector TargetLocation, ACharacter* Character)
{
	Character->SetActorLocation(TargetLocation);
}
