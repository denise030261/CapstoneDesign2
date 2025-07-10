// Fill out your copyright notice in the Description page of Project Settings.

#include "MoveSkill.h"
#include "Talisman.h"
#include "GameFramework/Character.h"
#include "FireAttribute.h"
#include "SpawnSkill.h"
#include <CapstoneDesign2/MainCharacter.h>

void UMoveSkill::SkillExecute_Implementation(ATalisman* Owner, UWorld* World)
{
	Owner->SetActorTickEnabled(true);
	UE_LOG(LogTemp, Warning, TEXT("Move Skill"));
}

void UMoveSkill::HitExecute_Implementation(UWorld* World, AActor* OtherActor, ATalisman* Talisman)
{
	ACharacter* Character = World->GetFirstPlayerController()->GetCharacter();
	if (Character == nullptr)
		return;

	Character->SetActorLocation(Talisman->GetActorLocation());

	if (UTalismanAttributeStrategy* AttributeCDO = Talisman->TalismanDataAsset->SkillInfo.Attribute->GetDefaultObject<UTalismanAttributeStrategy>())
		if (UFireAttribute* FireAttributeCDO = Cast<UFireAttribute>(AttributeCDO))
			FireDance(Talisman, World, Character);
}

void UMoveSkill::Moving(FVector TargetLocation, ACharacter* Character, ATalisman* Owner, UWorld* World)
{
	Character->SetActorLocation(TargetLocation);

	if(UTalismanAttributeStrategy* AttributeCDO = Owner->TalismanDataAsset->SkillInfo.Attribute->GetDefaultObject<UTalismanAttributeStrategy>())
		if (UFireAttribute* FireAttributeCDO = Cast<UFireAttribute>(AttributeCDO))
			FireDance(Owner, World, Character);
}

void UMoveSkill::FireDance(ATalisman* Owner, UWorld* World, ACharacter* Character)
{
	AMainCharacter* Player = Cast<AMainCharacter>(Character);
	if (Player==nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player"));
		return;
	}
	Player->Dancing();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (Owner && Owner->TalismanDataAsset && Owner->TalismanDataAsset->SkillInfo.SpawnSkill && World)
	{
		USceneComponent* ParentRoot = Character->GetMesh();

		ASpawnSkill* LeftSpawnedActor = World->SpawnActor<ASpawnSkill>(
			Owner->TalismanDataAsset->SkillInfo.SpawnSkill,
			FVector(0,-20,-1), // 임시 위치
			FRotator::ZeroRotator,
			SpawnParams
		);
		ASpawnSkill* RightSpawnedActor = World->SpawnActor<ASpawnSkill>(
			Owner->TalismanDataAsset->SkillInfo.SpawnSkill,
			FVector(0, -20, -1), // 임시 위치
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
