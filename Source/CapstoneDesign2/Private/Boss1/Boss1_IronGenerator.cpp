// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_IronGenerator.h"

#include "Boss1/Boss1.h"
#include "Boss1/Boss1_Iron.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "Components/SphereComponent.h"

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

	for (int32 i = 0; i < 5; i++)
	{
		if (ABoss1_Iron* Iron = GetIronFromPool())
		{
			const float Length = SpawnRadius * 0.9f;
			const float Angle = 2 * PI * i / 5;

			const float XPos = Length * FMath::Cos(Angle);
			const float YPos = Length * FMath::Sin(Angle);
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
	
	DrawDebugSphere(GetWorld(), GetActorLocation(), SpawnRadius, 12, FColor::Green, true);
}

void ABoss1_IronGenerator::GenerateIron()
{
	if (ABoss1_Iron* Iron = GetIronFromPool())
	{
		FVector SpawnPos;
		
		bool bNeedRegenerate;
		const float SweepRadius = Iron->SphereComponent->GetScaledSphereRadius() * 5.0f;
		do
		{
			bNeedRegenerate = false;
			
			const float RandLength = SpawnRadius * FMath::Sqrt(FMath::FRand());
			const float RandAngle = FMath::FRandRange(-PI, PI);

			const float XPos = RandLength * FMath::Cos(RandAngle);
			const float YPos = RandLength * FMath::Sin(RandAngle);
			SpawnPos = GetActorLocation() + FVector(XPos, YPos, 0.0f);

			TArray<FHitResult> HitResults;
			const FVector SweepStartPos = SpawnPos + FVector(0.0f, 0.0f, -1000.0f);
			const FVector SweepEndPos = SpawnPos + FVector(0.0f, 0.0f, 1000.0f);
			FCollisionQueryParams TraceParams(FName(TEXT("SphereTrace")), true, this); // "SphereTrace"는 디버그 이름, true는 복잡한 충돌, this는 자기 자신을 무시
			
			if (GetWorld()->SweepMultiByChannel(
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
					AActor* OtherActor = HitResult.GetActor();
					if (Cast<ABoss1_Iron>(OtherActor) || Cast<ABoss1>(OtherActor) || Cast<AMainCharacter>(OtherActor))
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