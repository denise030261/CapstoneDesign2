// Fill out your copyright notice in the Description page of Project Settings.

#include "NormalAttack.h"
#include "Talisman.h"

void UNormalAttack::SkillExecute_Implementation(ATalisman* Owner, UWorld* World) // 위치를 이동한다.
{
	OwnTalisman = Owner;
	OwnTalisman->SetActorTickEnabled(true);
	UE_LOG(LogTemp, Warning, TEXT("Normal Attack"));
}

void UNormalAttack::Debuff(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman)
{
	OwnTalisman = ThisTalisman;
	if (OwnTalisman->TalismanDataAsset->SkillInfo.SpawnSkill)
	{
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector SpawnLocation = OtherActor ? OtherActor->GetActorLocation() + FVector(0, 0, 0) : FVector(0, 0, 0);
			FRotator SpawnRotation = FRotator::ZeroRotator;

			if (OwnTalisman && OwnTalisman->TalismanDataAsset && OwnTalisman->TalismanDataAsset->SkillInfo.SpawnSkill && World)
			{
				AActor* SpawnedActor = World->SpawnActor<AActor>(
					OwnTalisman->TalismanDataAsset->SkillInfo.SpawnSkill,
					SpawnLocation,
					SpawnRotation,
					SpawnParams
				);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Spawn 실패!"));
			}


		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No World"));
		}
		UE_LOG(LogTemp, Warning, TEXT("SpawnSkill"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No SpawnSkill"));
	}
}
