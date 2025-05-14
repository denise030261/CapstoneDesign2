// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnSkill.h"
#include "NiagaraComponent.h"

// Sets default values
ASpawnSkill::ASpawnSkill()
{
	PrimaryActorTick.bCanEverTick = true;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));
	if (NiagaraComponent == nullptr)
		return;

	TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerVolume"));
	if (TriggerVolume)
	{
		RootComponent = TriggerVolume;
		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		TriggerVolume->SetGenerateOverlapEvents(true);
	}

	NiagaraComponent->SetupAttachment(RootComponent);
	// Niagara under static mesh
	SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void ASpawnSkill::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerVolume)
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpawnSkill::OnOverlapBegin);

	FTimerDelegate DestroyDelegate;

	DestroyDelegate = FTimerDelegate::CreateLambda([this]()
		{
			Destroy();
		});

	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, DestroyDelegate, DestroyTime, false);
}

void ASpawnSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector StartLocation = GetActorLocation();
	FVector MoveLocation = TargetDistance - StartLocation;

	if (MoveLocation.Size() > 1)
	{
		SetActorLocation(StartLocation + MoveLocation.GetSafeNormal() * Speed * DeltaTime);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Arrived, Destroying"));
		Destroy();
	}
} 

void ASpawnSkill::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Enemy Damage
}

void ASpawnSkill::SpawnMove(FVector3d StartLocation)
{
	TargetDistance = StartLocation;
	SetActorTickEnabled(true);
}