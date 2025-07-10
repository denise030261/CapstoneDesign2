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
		FVector ForwardDirection = FRotationMatrix(Owner->GetActorRotation()-FRotator(0,90,0)).GetUnitAxis(EAxis::X);
		FVector SpawnLocation = Owner->GetActorLocation() + ForwardDirection * Owner->TalismanDataAsset->SkillInfo.Distance;
		FVector RightVector = FRotationMatrix(Owner->GetActorRotation() - FRotator(0, 90, 0)).GetUnitAxis(EAxis::Y);
		FVector SpawnLeftLocation = SpawnLocation - RightVector * 300;
		FVector SpawnRightLocation = SpawnLocation + RightVector * 300;
		ASpawnSkill* SpawnedActor = World->SpawnActor<ASpawnSkill>(
			Owner->TalismanDataAsset->SkillInfo.SpawnSkill,
			SpawnLocation,
			Owner->GetActorRotation(),
			SpawnParams);

		SpawnedActor->SetTalisman(Owner);
		bool left = false;
		for (int i = 1; i <= SpawnedActor->SpawnCount; i++)
		{
			if (left)
			{
				ASpawnSkill* MoreSpawnActor = World->SpawnActor<ASpawnSkill>(
					Owner->TalismanDataAsset->SkillInfo.SpawnSkill,
					SpawnLeftLocation,
					Owner->GetActorRotation(),
					SpawnParams);
			}
			else
			{
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

void USpawnAttack::HitExecute_Implementation(UWorld* World, AActor* OtherActor, ATalisman* Talisman)
{
	;
}
