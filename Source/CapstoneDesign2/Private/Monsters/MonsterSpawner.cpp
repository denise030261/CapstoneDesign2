// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/MonsterSpawner.h"

#include "CapstoneDesign2/MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Monsters/MonsterBase.h"

// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(FName("SceneComponent"));
	RootComponent = SceneComponent;
}

void AMonsterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckMonsterCount();
}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < MaxMonsterCount; i++)
	{
		GenerateMonster();
	}
	
	DrawDebugSphere(GetWorld(), GetActorLocation(), SpawnRadius, 12, FColor::Green, true);
}

void AMonsterSpawner::GenerateMonster()
{
	FVector SpawnPos;
	
	bool bNeedRegenerate;
	const float SweepRadius = MonsterToSpawn.GetDefaultObject()->GetCapsuleComponent()->GetScaledCapsuleRadius() * 5.0f;
	do
	{
		bNeedRegenerate = false;
		
		const float RandLength = SpawnRadius * FMath::Sqrt(FMath::FRand());
		const float RandAngle = FMath::FRandRange(-PI, PI);

		const float XPos = RandLength * FMath::Cos(RandAngle);
		const float YPos = RandLength * FMath::Sin(RandAngle);
		const float ZPos = MonsterToSpawn.GetDefaultObject()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		SpawnPos = GetActorLocation() + FVector(XPos, YPos, ZPos);

		const FVector SweepStartPos = SpawnPos + FVector(0.0f, 0.0f, -1000.0f);
		const FVector SweepEndPos = SpawnPos + FVector(0.0f, 0.0f, 1000.0f);
		FCollisionQueryParams TraceParams(FName(TEXT("SphereTrace")), true, this); // "SphereTrace"는 디버그 이름, true는 복잡한 충돌, this는 자기 자신을 무시
		
		if (TArray<FHitResult> HitResults; GetWorld()->SweepMultiByChannel(
			HitResults,
			SweepStartPos,
			SweepEndPos,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(SweepRadius),
			TraceParams
		))
		{
			for (const FHitResult& HitResult : HitResults)
			{
				if (AActor* OtherActor = HitResult.GetActor(); Cast<AMonsterBase>(OtherActor) || Cast<AMainCharacter>(OtherActor))
				{
					bNeedRegenerate = true;
					break;
				}
			}
		}
	}
	while (bNeedRegenerate);
	
	const float RotAngle = FMath::FRandRange(0.0f, 360.0f);
	const FRotator SpawnRot = FRotator(0.0f, RotAngle, 0.0f);

	AMonsterBase* Monster = GetWorld()->SpawnActor<AMonsterBase>(MonsterToSpawn, SpawnPos, SpawnRot);
	if (Monster)
	{
		Monsters.Add(Monster);
#if WITH_EDITOR
		Monster->SetFolderPath(FName("Monsters"));
#endif
	}
}

void AMonsterSpawner::CheckMonsterCount()
{
	const int32 RemovedCount = Monsters.RemoveAll([](const AMonsterBase* Monster)
	{
		return !IsValid(Monster);
	});

	for (int32 i = 0; i < RemovedCount; i++)
	{
		FTimerHandle ReGenerateHandle;
		GetWorldTimerManager().SetTimer(ReGenerateHandle, this, &AMonsterSpawner::GenerateMonster, ReGenerateCoolTime, false);
	}
}
