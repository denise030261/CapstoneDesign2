// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnSkill.h"
#include "NiagaraComponent.h"
#include <Damageable.h>
#include "Talisman.h"

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

	if (MoveLocation.Size() > 5)
	{
		SetActorLocation(StartLocation + MoveLocation.GetSafeNormal() * Speed * DeltaTime);
	}
	else
	{
		Destroy();
	}
}

void ASpawnSkill::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
	{
		RepeatAttack(OtherActor);

		FTimerDelegate AttackDelegate;
		AttackDelegate.BindUFunction(this, FName("RepeatAttack"), OtherActor);
		GetWorld()->GetTimerManager().SetTimer(AttackHandle, AttackDelegate, StayTime, false);
	}
}

void ASpawnSkill::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IDamageable* InterfaceRef = Cast<IDamageable>(OtherActor);
	if (InterfaceRef)
	{
		if (TalismanDataAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Done Attack"), *GetName());
			GetWorld()->GetTimerManager().ClearTimer(AttackHandle);
		}
	}
}

void ASpawnSkill::RepeatAttack(AActor* OtherActor)
{
	IDamageable* InterfaceRef = Cast<IDamageable>(OtherActor);
	if (InterfaceRef)
	{
		if (TalismanDataAsset)
		{
			InterfaceRef->DealDamage(TalismanDataAsset->SkillInfo.Damage, TalismanDataAsset);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No TalismanDataAsset"));
		}
	}
}

void ASpawnSkill::SpawnMove(FVector3d StartLocation)
{
	TargetDistance = StartLocation;
	SetActorTickEnabled(true);
}

void ASpawnSkill::SetTalisman(ATalisman* Talisman)
{
	OwnTalisman = Talisman;
}
