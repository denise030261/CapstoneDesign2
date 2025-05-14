// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalAttribute.h"
#include "GameFramework/CharacterMovementComponent.h"

void UNormalAttribute::Attack_Implementation(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman)
{
	UE_LOG(LogTemp, Warning, TEXT("Normal Attribute"));
}

void UNormalAttribute::Passive_Implementation(UWorld* World)
{
	FTimerHandle TempHandle;
	Character = World->GetFirstPlayerController()->GetCharacter();
	if (Character == nullptr)
		return;

	if (Character)
		Character->JumpMaxHoldTime = 0.5f;
	World->GetTimerManager().SetTimer(TempHandle, [this]()
		{
			if (Character)
				Character->JumpMaxHoldTime = 0;
		}, 5, false);
}
