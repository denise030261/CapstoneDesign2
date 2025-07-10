// Fill out your copyright notice in the Description page of Project Settings.


#include "FireAttribute.h"
#include "Talisman.h"
#include <Kismet/GameplayStatics.h>

UFireAttribute::UFireAttribute()
{
	ConstructorHelpers::FClassFinder<ASpawnSkill> SkillBPClass(TEXT("/Game/CapstoneDesign/Blueprints/SpawnActor/BP_SpawnSmallFire"));
	if (SkillBPClass.Succeeded())
	{
		FireActor = SkillBPClass.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed"));
	}
}

void UFireAttribute::Attack_Implementation(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman)
{
	if (World)
	{
		if (FireActor)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector SpawnLocation = OtherActor ? OtherActor->GetActorLocation() + FVector(0, 0, 0) : FVector(0, 0, 0);
			FRotator SpawnRotation = FRotator::ZeroRotator;

			ASpawnSkill* SpawnedActor = World->SpawnActor<ASpawnSkill>(
				FireActor,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);

			if (SpawnedActor && OtherActor)
			{
				USceneComponent* AttachPoint = OtherActor->GetRootComponent();
				if (AttachPoint)
				{
					SpawnedActor->AttachToComponent(AttachPoint, FAttachmentTransformRules::KeepWorldTransform);
				}
			}

			if (!SpawnedActor)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn ASpawnSkill!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FireActor is null!"));
		}


	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No World"));
	}
}

void UFireAttribute::Passive_Implementation(UWorld* World)
{
	;
}


