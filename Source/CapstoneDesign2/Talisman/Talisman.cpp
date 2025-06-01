// Fill out your copyright notice in the Description page of Project Settings.


#include "Talisman.h"
#include "RangeAttack.h"
#include "NormalAttack.h"
#include "FireAttribute.h"
#include "NormalAttribute.h"
#include "MoveSkill.h"

// Sets default values
ATalisman::ATalisman()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TalismanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TalismanMesh"));
	if (TalismanMesh == nullptr)
		return;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	if (TriggerVolume == nullptr)
		return;
	
	RootComponent = TalismanMesh;
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetGenerateOverlapEvents(true);
	TriggerVolume->SetupAttachment(TalismanMesh);
}

void ATalisman::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetActorTickEnabled(false); // Don't Tick

	if (ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Detect Player"));
		return;
	}
	//else if(ActorHasTag(""))
	
	if (TalismanDataAsset->SkillInfo.Skill)
	{
		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision); // For No Call OverlapBegin()

		UTalismanSkillStrategy* SkillCDO = TalismanDataAsset->SkillInfo.Skill->GetDefaultObject<UTalismanSkillStrategy>();
		UTalismanAttributeStrategy* AttributeCDO = TalismanDataAsset->SkillInfo.Attribute->GetDefaultObject<UTalismanAttributeStrategy>();

		AttributeCDO->Attack(GetWorld(), OtherActor, this);

		if (URangeAttack* RangeAttackCDO = Cast<URangeAttack>(SkillCDO))
		{
			if (UFireAttribute* FireAttackCDO = Cast<UFireAttribute>(AttributeCDO))
			{
				RangeAttackCDO->BombAttack(GetWorld(), OtherActor, this);
			}
			else if (UNormalAttribute* NormalAttackCDO = Cast<UNormalAttribute>(AttributeCDO))
			{
				RangeAttackCDO->DuplicateAttack(GetWorld(), OtherActor, this);
			}

			return;
		}
		else if (UMoveSkill* MoveSKillCDO = Cast<UMoveSkill>(SkillCDO))
		{
			MoveSKillCDO->Moving(GetActorLocation(),GetWorld()->GetFirstPlayerController()->GetCharacter(),this, GetWorld());
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Destroy"));
	Destroy();
	//Other Actor가 본인 캐릭터 외에 감지되면 파괴 적이면 데미지 입히기
	// 범위 공격은 범위를 스폰
}

void ATalisman::SetMoveDistance(FVector3d StartLocation)
{
	TargetDistance = StartLocation;
}

// Called when the game starts or when spawned
void ATalisman::BeginPlay()
{
	Super::BeginPlay();

	if(TriggerVolume)
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATalisman::OnOverlapBegin);
}

// Called every frame
void ATalisman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector StartLocation = GetActorLocation();
	FVector MoveLocation = TargetDistance - StartLocation;

	if (MoveLocation.Size() > 10)
	{
		SetActorLocation(StartLocation + MoveLocation.GetSafeNormal() * Speed * DeltaTime);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Arrived, Destroying"));
		UTalismanSkillStrategy* SkillCDO = TalismanDataAsset->SkillInfo.Skill->GetDefaultObject<UTalismanSkillStrategy>();
		if (UMoveSkill* MoveSKillCDO = Cast<UMoveSkill>(SkillCDO))
		{
			MoveSKillCDO->Moving(GetActorLocation(), GetWorld()->GetFirstPlayerController()->GetCharacter(), this, GetWorld());
		}
		Destroy();
	}
}

