#include "RangeAttack.h"
#include "TimerManager.h"
#include "Talisman.h"
#include "NiagaraSystem.h"
#include <Kismet/GameplayStatics.h>
#include <NiagaraFunctionLibrary.h>
#include "SpawnSkill.h"
#include <Damageable.h>

void URangeAttack::SkillExecute_Implementation(ATalisman* Owner, UWorld* World)
{
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Talisman"));
		return;
	}
	Owner->SetActorTickEnabled(true);
	Owner->bRangeAttack = true;
	UE_LOG(LogTemp, Warning, TEXT("Range Attack"));
}

void URangeAttack::BombAttack(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman)
{
	if (!World || !OtherActor || !ThisTalisman)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters"));
		return;
	}

	ThisTalisman->AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
	ThisTalisman->SetActorRotation(FRotator(90, 90, 0));

	FTimerHandle BombTimerHandle;
	FTimerDelegate BombDelegate;

	BombDelegate.BindUObject(this, &URangeAttack::Bomb, ThisTalisman, OtherActor);
	World->GetTimerManager().SetTimer(BombTimerHandle, BombDelegate, 3.0f, false);
}

void URangeAttack::Bomb(ATalisman* Talisman, AActor* Target)
{
	if (!Talisman)
	{
		UE_LOG(LogTemp, Error, TEXT("Talisman is null"));
		return;
	}

	if (!Talisman->TalismanDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("TalismanDataAsset is null"));
		return;
	}

	USceneComponent* AttachComponent = Target ? Target->GetRootComponent() : nullptr;
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		Talisman->TalismanDataAsset->SkillInfo.ExecuteEffect,
		AttachComponent,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true
	);

	IDamageable* InterfaceRef = Cast<IDamageable>(Target);
	if (InterfaceRef)
	{
		InterfaceRef->DealDamage(Talisman->TalismanDataAsset->SkillInfo.Damage, Talisman->TalismanDataAsset);
	}

	Talisman->Destroy();
}

void URangeAttack::DuplicateAttack(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman)
{
	ThisTalisman->SetActorHiddenInGame(true);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ThisTalisman && ThisTalisman->TalismanDataAsset && ThisTalisman->TalismanDataAsset->SkillInfo.SpawnSkill && World)
	{
		float Distance = ThisTalisman->TalismanDataAsset->SkillInfo.Distance;
		FVector OriginLocation = ThisTalisman->GetActorLocation();
		FVector ForwardVector = ThisTalisman->GetActorForwardVector();
		FVector RightVector = ThisTalisman->GetActorRightVector();

		FVector Locations[4];
		Locations[0] = OriginLocation + ForwardVector * Distance;
		Locations[1] = OriginLocation - ForwardVector * Distance;
		Locations[2] = OriginLocation - RightVector * Distance;
		Locations[3] = OriginLocation + RightVector * Distance;

		for (int i = 0; i < 4; i++)
		{
			FRotator SpawnRotator = ThisTalisman->GetActorRotation();
			if (i == 0 || i == 1)
			{
				SpawnRotator += FRotator(0, 90, 0);
			}
			ASpawnSkill* SpawnedActor = World->SpawnActor<ASpawnSkill>(
				ThisTalisman->TalismanDataAsset->SkillInfo.SpawnSkill,
				ThisTalisman->GetActorLocation(),
				SpawnRotator,
				SpawnParams);

			if(SpawnedActor)
				SpawnedActor->SpawnMove(Locations[i]);
		}

		ThisTalisman->Destroy();
	}
}
