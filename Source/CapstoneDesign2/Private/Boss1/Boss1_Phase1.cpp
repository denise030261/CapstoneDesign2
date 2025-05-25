// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss1/Boss1_Phase1.h"

#include "Boss1/Boss1Anim.h"
#include "Boss1/Boss1_IronGenerator.h"
#include "Boss1/Boss1_Phase2.h"
#include "Boss1/Boss1_Projectile_Needle.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "CapstoneDesign2/Talisman/FireAttribute.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABoss1_Phase1::ABoss1_Phase1()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonGrux/Characters/Heroes/Grux/Meshes/Grux.Grux'"));
	if (MeshAsset.Succeeded()) GetMesh()->SetSkeletalMesh(MeshAsset.Object);
}

// Called when the game starts or when spawned
void ABoss1_Phase1::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnEndOnceNotify.AddDynamic(this, &ABoss1_Phase1::ThrowMassEnd);

	//PlayerCharacter =  //플레이어 저장
	IronGenerator = GetWorld()->SpawnActor<ABoss1_IronGenerator>(IronGenerator->StaticClass(), GetActorLocation(), GetActorRotation());
	
	GetWorldTimerManager().SetTimer(Phase1TimerHandle, this, &ABoss1_Phase1::SetToPhase2, Phase1Second, false);

	FTimerHandle SpawnHandle;
	constexpr float SpawnTime = 2.0f;
	GetWorldTimerManager().SetTimer(SpawnHandle, this, &ABoss1_Phase1::SetStateIdle, SpawnTime, false);
}

// Called every frame
void ABoss1_Phase1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoss1_Phase1::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
	}
}

void ABoss1_Phase1::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (ABoss1_Iron* Iron = Cast<ABoss1_Iron>(OtherActor))
		{
			EatIron(Iron);
			SetStateIdle();
			if (NowIronCount >= MaxIronCount)
			{
				SetToPhase2();
			}
		}
	}
}

void ABoss1_Phase1::CheckState(float DeltaTime)
{
	switch (State)
	{
	case EBoss1_State::Tracing:
		MoveToIron(DeltaTime);
		break;

	case EBoss1_State::Aiming:
		Aiming(DeltaTime);
		break;
		
	default:
		break;
	}
	
}

void ABoss1_Phase1::Trace(float DeltaTime)
{
	MoveToIron(DeltaTime);
}

void ABoss1_Phase1::ShootNeedle()
{
	if (PatternState == EBoss1_Pattern_State::ShootNeedle)
	{
		State = EBoss1_State::Casting;
		const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerCharacter->GetActorLocation());
		ABoss1_Projectile_Needle* Needle = GetWorld()->SpawnActor<ABoss1_Projectile_Needle>(NeedleProjectile, GetActorLocation(), LookAt);
		Needle->Damage = ShootNeeleDamage;
		Needle->SetActorRelativeScale3D(Needle->GetActorRelativeScale3D() * FMath::Pow(EatIronScaleFactor, NowIronCount));
#if WITH_EDITOR
		Needle->SetFolderPath(FName("Projectiles"));
#endif
	}
}

void ABoss1_Phase1::SetToPhase2()
{
	GetWorldTimerManager().ClearTimer(Phase1TimerHandle);
	if (auto c= GetWorld()->SpawnActor<ABoss1_Phase2>(Boss1_Phase2ToSpawn, GetActorLocation(), GetActorRotation()))
	{
		c->NowIronCount = NowIronCount;
		c->PlayerCharacter = PlayerCharacter;
		c->IronGenerator = IronGenerator;
		c->GetRootComponent()->SetRelativeScale3D(GetRootComponent()->GetRelativeScale3D());
	}
	
	Destroy();
}

void ABoss1_Phase1::DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset)
{
	if (Cast<UFireAttribute>(DataAsset->SkillInfo.Attribute))
	{
		UE_LOG(LogTemp, Warning, TEXT("불 히트!"));
		
		if (State == EBoss1_State::Idle || State == EBoss1_State::Tracing)
		{
			if (FMath::FRand() * (ShootNeedleProb + ThrowMassProb) < ShootNeedleProb)
			{
				ShootNeedleStart();
			}
			else
			{
				ThrowMassStart();
			}
		}
	}
	else
	{
		
	}
}
