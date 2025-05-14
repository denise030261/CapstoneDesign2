// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnAttack.h"
#include "Talisman.h"
#include "SpawnSkill.h"

void USpawnAttack::SkillExecute_Implementation(ATalisman* Owner, UWorld* World)
{
	Owner->SetActorHiddenInGame(true);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (Owner && Owner->TalismanDataAsset && Owner->TalismanDataAsset->SkillInfo.SpawnSkill && World)
	{
		FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * Owner->TalismanDataAsset->SkillInfo.Distance;
		FVector RightVecotr = Owner->GetActorRightVector();
		FVector SpawnLeftLocation = SpawnLocation - RightVecotr * 300;
		FVector SpawnRightLocation = SpawnLocation + RightVecotr * 300;
		ASpawnSkill* SpawnedActor = World->SpawnActor<ASpawnSkill>(
			Owner->TalismanDataAsset->SkillInfo.SpawnSkill,
			SpawnLocation,
			Owner->GetActorRotation(),
			SpawnParams);

		bool left = false;
		for (int i = 1; i <= SpawnedActor->SpawnCount; i++)
		{
			if (left)
			{
				UE_LOG(LogTemp, Warning, TEXT("Left"));
				ASpawnSkill* MoreSpawnActor = World->SpawnActor<ASpawnSkill>(
					Owner->TalismanDataAsset->SkillInfo.SpawnSkill,
					SpawnLeftLocation,
					Owner->GetActorRotation(),
					SpawnParams);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Right"));
				ASpawnSkill* MoreSpawnActor = World->SpawnActor<ASpawnSkill>(
					Owner->TalismanDataAsset->SkillInfo.SpawnSkill,
					SpawnRightLocation,
					Owner->GetActorRotation(),
					SpawnParams);
			}

			left = !left;
		}
	}
}
