// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1GameModeHelper.h"

#include "EngineUtils.h"
#include "Boss1/Boss1_Phase1.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBoss1GameModeHelper::UBoss1GameModeHelper()
{
	static ConstructorHelpers::FClassFinder<AActor> SpawnPointActorFinder(TEXT("/Script/Engine.Blueprint'/Game/CapstoneDesign/Blueprints/Boss/BossSpawnPoint.BossSpawnPoint_C'"));
	if (SpawnPointActorFinder.Succeeded()) SpawnPointActor = SpawnPointActorFinder.Class;
}

FVector UBoss1GameModeHelper::FindBossSpawnLocation() const
{
	FVector SpawnLocation;
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* CurrentActor  = *It;
		if (CurrentActor && CurrentActor->GetClass() == SpawnPointActor.Get())
		{
			SpawnLocation = CurrentActor->GetActorLocation();
			break;
		}
	}
	return SpawnLocation;
}

FRotator UBoss1GameModeHelper::FindBossSpawnRotation() const
{
	const FVector BossLocation = FindBossSpawnLocation();
	const FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(BossLocation, PlayerLocation);
	
	return FRotator(0.0f, LookAt.Yaw, 0.0f);
}

FRotator UBoss1GameModeHelper::FindPlayerSpawnRotation() const
{
	const FVector BossLocation = FindBossSpawnLocation();
	const FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, BossLocation);
	
	return FRotator(0.0f, LookAt.Yaw, 0.0f);
}

ABoss1_Phase1* UBoss1GameModeHelper::GenerateBoss1(FVector SpawnLocation, FRotator SpawnRotation)
{
	Boss1 = GetWorld()->SpawnActor<ABoss1_Phase1>(ABoss1_Phase1::StaticClass(), SpawnLocation, SpawnRotation);

	if (Boss1)
	{
		Boss1->PlayerCharacter = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		return Boss1;
	}
	else
	{
		return GenerateBoss1(SpawnLocation, SpawnRotation);
	}
}

void UBoss1GameModeHelper::SetStage()
{
	const FVector BossLocation = FindBossSpawnLocation();
	const FRotator BossRotation = FindBossSpawnRotation();

	GenerateBoss1(BossLocation, BossRotation);

	const FRotator PlayerRotation = FindPlayerSpawnRotation();
	
	GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorRotation(PlayerRotation);
	GetWorld()->GetFirstPlayerController()->GetPawn()->GetController()->SetControlRotation(PlayerRotation);
}
