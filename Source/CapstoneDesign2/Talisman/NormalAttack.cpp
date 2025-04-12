// Fill out your copyright notice in the Description page of Project Settings.

#include "NormalAttack.h"
#include "Talisman.h"

void UNormalAttack::SkillExecute_Implementation(ATalisman* Owner) // 위치를 이동한다.
{
	OwnTalisman = Owner;
	OwnTalisman->SetActorTickEnabled(true);
	UE_LOG(LogTemp, Warning, TEXT("Normal Attack"));
}
