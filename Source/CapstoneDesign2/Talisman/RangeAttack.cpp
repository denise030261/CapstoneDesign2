#include "RangeAttack.h"
#include "TimerManager.h"
#include "Talisman.h"
#include "NiagaraSystem.h"
#include <Kismet/GameplayStatics.h>
#include <NiagaraFunctionLibrary.h>

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

	ThisTalisman->SetActorEnableCollision(false);
	FVector TalisManLocation(0, -20, 10);
	TalisManLocation += OtherActor->GetActorLocation();
	ThisTalisman->SetActorLocation(TalisManLocation);
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

	UE_LOG(LogTemp, Warning, TEXT("Bomb"));
	UE_LOG(LogTemp, Warning, TEXT("%s : Talisman"), *Talisman->GetName());
	Talisman->Destroy();
}
