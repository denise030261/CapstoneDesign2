// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveAttack.h"
#include "Talisman.h"
#include "GameFramework/Character.h"
#include "SpawnSkill.h"

void UMoveAttack::SkillExecute_Implementation(ATalisman* Owner, UWorld* World)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ACharacter* Character = World->GetFirstPlayerController()->GetCharacter();
	if (Owner && Owner->TalismanDataAsset && Owner->TalismanDataAsset->SkillInfo.SpawnSkill && World && Character)
	{
		USceneComponent* ParentRoot = Character->GetMesh();

		ASpawnSkill* LeftSpawnedActor = World->SpawnActor<ASpawnSkill>(
			Owner->TalismanDataAsset->SkillInfo.SpawnSkill,
			FVector::ZeroVector, // 임시 위치
			FRotator::ZeroRotator,
			SpawnParams
		);
		ASpawnSkill* RightSpawnedActor = World->SpawnActor<ASpawnSkill>(
			Owner->TalismanDataAsset->SkillInfo.SpawnSkill,
			FVector::ZeroVector, // 임시 위치
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (LeftSpawnedActor && RightSpawnedActor)
		{
			LeftSpawnedActor->AttachToComponent(ParentRoot, FAttachmentTransformRules::KeepRelativeTransform, FName("LeftHand"));
			RightSpawnedActor->AttachToComponent(ParentRoot, FAttachmentTransformRules::KeepRelativeTransform, FName("RightHand"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No SpawnActor"));
		}
	}
}
