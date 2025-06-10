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
	if (OtherActor->ActorHasTag("Player"))
	{
		return;
	}

	IDamageable* InterfaceRef = Cast<IDamageable>(OtherActor);
	if (OtherActor && InterfaceRef)
	{
		OverlappingDamageableActors.Add(OtherActor);

		if (!GetWorld()->GetTimerManager().IsTimerActive(AttackHandle))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: First actor entered, starting attack timer. Target: %s"), *GetName(), *OtherActor->GetName());

			GetWorld()->GetTimerManager().SetTimer(
				AttackHandle,
				this,
				&ASpawnSkill::RepeatAttack, 
				StayTime, 
				true 
			);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: Another actor entered, timer already active. Current active targets: %d"), *GetName(), OverlappingDamageableActors.Num());
			return;
		}

		InterfaceRef->DealDamage(TalismanDataAsset->SkillInfo.Damage, TalismanDataAsset);
	}
}

void ASpawnSkill::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IDamageable* InterfaceRef = Cast<IDamageable>(OtherActor);
	if (OtherActor && InterfaceRef)
	{
		OverlappingDamageableActors.Remove(OtherActor);
		UE_LOG(LogTemp, Log, TEXT("%s: Actor %s left. Remaining targets: %d"), *GetName(), *OtherActor->GetName(), OverlappingDamageableActors.Num());

		if (OverlappingDamageableActors.Num() == 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(AttackHandle);
		}
	}
}

void ASpawnSkill::RepeatAttack()
{
	if (!TalismanDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("TalismanDataAsset is null in RepeatAttack!"));
		GetWorld()->GetTimerManager().ClearTimer(AttackHandle);
		return;
	}

	int32 DamagedCount = 0;
	for (AActor* Target : OverlappingDamageableActors) 
	{
		if (Target && Target->IsValidLowLevel()) 
		{
			IDamageable* InterfaceRef = Cast<IDamageable>(Target);
			if (InterfaceRef)
			{
				InterfaceRef->DealDamage(TalismanDataAsset->SkillInfo.Damage, TalismanDataAsset);
				DamagedCount++;
			}
		}
	}

	if (OverlappingDamageableActors.Num() == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackHandle);
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
