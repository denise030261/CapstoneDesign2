// Fill out your copyright notice in the Description page of Project Settings.


#include "FireAttribute.h"
#include "Talisman.h"

void UFireAttribute::Attack_Implementation(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman)
{
	if (ThisTalisman->TalismanDataAsset->SkillInfo.SpawnSkill)
	{
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector SpawnLocation = OtherActor ? OtherActor->GetActorLocation() + FVector(0, 0, 0) : FVector(0, 0, 0);
			FRotator SpawnRotation = FRotator::ZeroRotator;

			if (ThisTalisman && ThisTalisman->TalismanDataAsset && ThisTalisman->TalismanDataAsset->SkillInfo.SpawnSkill && World)
			{
				AActor* SpawnedActor = World->SpawnActor<AActor>(
					ThisTalisman->TalismanDataAsset->SkillInfo.SpawnSkill,
					SpawnLocation,
					SpawnRotation,
					SpawnParams
				);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Spawn ½ÇÆÐ!"));
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

void UFireAttribute::Passive_Implementation(UWorld* World)
{
	;
}
