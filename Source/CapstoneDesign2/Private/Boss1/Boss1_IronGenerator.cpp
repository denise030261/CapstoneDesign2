// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_IronGenerator.h"

#include "Boss1/Boss1_Iron.h"

// Sets default values
ABoss1_IronGenerator::ABoss1_IronGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(FName("SceneComponent"));
	RootComponent = SceneComponent;
}

// Called when the game starts or when spawned
void ABoss1_IronGenerator::BeginPlay()
{
	Super::BeginPlay();

	InitializeIronPool();
	
	GetWorldTimerManager().SetTimer(GenerateTimerHandle, this, &ABoss1_IronGenerator::GenerateIron, GenerateCoolTime, true); // 쿨타임마다 쇠 생성
	
	DrawDebugSphere(GetWorld(), GetActorLocation(), SpawnRadius, 12, FColor::Green, true);
}

void ABoss1_IronGenerator::GenerateIron()
{
	if (ABoss1_Iron* Iron = GetIronFromPool())
	{
		const float RandLength = SpawnRadius * FMath::Sqrt(FMath::FRand());
		const float RandAngle = FMath::FRandRange(-PI, PI);

		const float XPos = RandLength * FMath::Cos(RandAngle);
		const float YPos = RandLength * FMath::Sin(RandAngle);
		const FVector SpawnPos = GetActorLocation() + FVector(XPos, YPos, 0.0f);
	
		const float RotAngle = FMath::FRandRange(0.0f, 360.0f);
		const FRotator SpawnRot = FRotator(0.0f, RotAngle, 0.0f);

		Iron->Enable(SpawnPos, SpawnRot);
		SpawnedIrons.Add(Iron);
#if WITH_EDITOR
		Iron->SetFolderPath(FName("Irons/Spawned Irons"));
#endif
	}
}

void ABoss1_IronGenerator::InitializeIronPool()
{
	for (int32 i = 0; i < MaxIronCount; i++)
	{
		if (ABoss1_Iron* Iron = GetWorld()->SpawnActor<ABoss1_Iron>(IronToSpawn))
		{
			Iron->Disable();
			IronPool.Push(Iron);

#if WITH_EDITOR
			Iron->SetFolderPath(FName("Irons/Iron Pool"));
#endif
		}
	}
}

ABoss1_Iron* ABoss1_IronGenerator::GetIronFromPool()
{
	if (!IronPool.IsEmpty())
	{
		return IronPool.Pop();
	}
	return nullptr;
}

void ABoss1_IronGenerator::RemoveIron(ABoss1_Iron* Iron)
{
	if (SpawnedIrons.Remove(Iron))
	{
		Iron->Disable();
		IronPool.Push(Iron);

#if WITH_EDITOR
		Iron->SetFolderPath(FName("Irons/Iron Pool"));
#endif
	}
}