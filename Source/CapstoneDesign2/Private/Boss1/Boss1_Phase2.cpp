// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_Phase2.h"

#include "Boss1/Boss1Anim.h"
#include "Boss1/Boss1_Iron.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

class ABoss1_Iron;
// Sets default values
ABoss1_Phase2::ABoss1_Phase2()
{
	WeaponColliderL = CreateDefaultSubobject<UBoxComponent>(FName("WeaponCollider_L"));
	WeaponColliderL->OnComponentBeginOverlap.AddDynamic(this, &ABoss1_Phase2::OnOverlapBegin_Weapon);
	WeaponColliderL->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));
	WeaponColliderL->SetupAttachment(GetMesh());

	WeaponColliderR = CreateDefaultSubobject<UBoxComponent>(FName("WeaponCollider_R"));
	WeaponColliderR->OnComponentBeginOverlap.AddDynamic(this, &ABoss1_Phase2::OnOverlapBegin_Weapon);
	WeaponColliderR->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));
	WeaponColliderR->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimMeleeAttackMontageL(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Boss/Boss1/AM_Boss1_MeleeAttack_L.AM_Boss1_MeleeAttack_L'"));
	if (AnimMeleeAttackMontageL.Succeeded()) MeleeAttackMontages.Add(AnimMeleeAttackMontageL.Object);
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimMeleeAttackMontageR(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Boss/Boss1/AM_Boss1_MeleeAttack_R.AM_Boss1_MeleeAttack_R'"));
	if (AnimMeleeAttackMontageR.Succeeded()) MeleeAttackMontages.Add(AnimMeleeAttackMontageR.Object);
}

// Called when the game starts or when spawned
void ABoss1_Phase2::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnEndAllNotify.AddDynamic(this, &ABoss1_Phase2::ThrowMassEnd);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnMeleeAttackStartNotify.AddDynamic(this, &ABoss1_Phase2::MeleeAttack);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnMeleeAttackEndNotify.AddDynamic(this, &ABoss1_Phase2::MeleeAttackEnd);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnMeleeAttackDelayEndNotify.AddDynamic(this, &ABoss1_Phase2::MeleeAttackDelayEnd);
	
	//PlayerCharacter =  //플레이어 저장

	FTimerHandle SpawnHandle;
	float SpawnTime = 2.0f;
	GetWorldTimerManager().SetTimer(SpawnHandle, this, &ABoss1_Phase2::SetStateIdle, SpawnTime, false);
}

// Called every frame
void ABoss1_Phase2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector WeaponLStartPos = GetMesh()->GetSocketLocation(FName("FX_Trail_L_02"));
	const FVector WeaponLEndPos = GetMesh()->GetSocketLocation(FName("FX_Trail_L_01"));
	const FVector WeaponLLocation = (WeaponLStartPos + WeaponLEndPos) / 2.0f;
	FRotator WeaponLRotation = (WeaponLEndPos - WeaponLStartPos).Rotation();
	WeaponLRotation.Pitch += 90;
	
	WeaponColliderL->SetWorldLocation(WeaponLLocation);
	WeaponColliderL->SetWorldRotation(WeaponLRotation);

	const FVector WeaponRStartPos = GetMesh()->GetSocketLocation(FName("FX_Trail_R_02"));
	const FVector WeaponREndPos = GetMesh()->GetSocketLocation(FName("FX_Trail_R_01"));
	const FVector WeaponRLocation = (WeaponRStartPos + WeaponREndPos) / 2.0f;
	FRotator WeaponRRotation = (WeaponREndPos - WeaponRStartPos).Rotation();
	WeaponRRotation.Pitch += 90;
	
	WeaponColliderR->SetWorldLocation(WeaponRLocation);
	WeaponColliderR->SetWorldRotation(WeaponRRotation);

	//잘 따라다니는지 체크 필요
	
	DrawDebugBox(GetWorld(),
		WeaponColliderL->GetComponentLocation(),
		WeaponColliderL->GetScaledBoxExtent(),
		WeaponColliderL->GetComponentQuat(),
		FColor::Green, false, -1.f, 0, 2.f);

	DrawDebugBox(GetWorld(),
		WeaponColliderR->GetComponentLocation(),
		WeaponColliderR->GetScaledBoxExtent(),
		WeaponColliderR->GetComponentQuat(),
		FColor::Green, false, -1.f, 0, 2.f);
}

void ABoss1_Phase2::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
	}
}

void ABoss1_Phase2::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (ABoss1_Iron* Iron = Cast<ABoss1_Iron>(OtherActor))
		{
			EatIron(Iron);
			SetStateIdle();
		}
	}
}

void ABoss1_Phase2::OnOverlapBegin_Weapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
		{
			if (CanDamageMeleeAttack)
			{
				// Damage To Player
				CanDamageMeleeAttack = false;
			}
		}
	}
}

void ABoss1_Phase2::CheckState(float DeltaTime)
{
	switch (State)
	{
	case EBoss1_State::Tracing:
		Trace(DeltaTime);
		break;

	case EBoss1_State::Aiming:
		Aiming(DeltaTime);
		break;
		
	default:
		break;
	}
	
}

void ABoss1_Phase2::Trace(float DeltaTime)
{
	float Prob_Tick = 1 - FMath::Pow(1 - (ShootNeedleProb + ThrowMassProb + MeleeAttackProb), DeltaTime);

	if (FMath::FRand() < Prob_Tick)
	{
		float Rand = FMath::FRandRange(0.0f, ShootNeedleProb + ThrowMassProb + MeleeAttackProb);
		if (Rand < ShootNeedleProb)
		{
			ShootNeedleStart();
		}
		else if (Rand < ShootNeedleProb + ThrowMassProb)
		{
			ThrowMassStart();
		} else
		{
			MeleeAttackStart();
		}
	}
	else
	{
		MoveToIron(DeltaTime);
	}
}

void ABoss1_Phase2::ShootNeedle()
{
	if (PatternState == EBoss1_Pattern_State::ShootNeedle)
	{
		State = EBoss1_State::Casting;

		TArray<float> Rots = { -15.0f, -30.0f, 0.0f, 15.0f, 30.0f };
	
		for (int32 i = 0; i < Rots.Num(); i++)
		{
			ABoss1_Projectile_Needle* Needle = GetWorld()->SpawnActor<ABoss1_Projectile_Needle>(NeedleProjectile, GetActorLocation(), GetActorRotation() + FRotator(0.0f, Rots[i], 0.0f));
			Needle->SetActorRelativeScale3D(Needle->GetActorRelativeScale3D() * FMath::Pow(1.1f, NowIronCount));
#if WITH_EDITOR
			Needle->SetFolderPath(FName("Projectiles"));
#endif
		}
	}
}

void ABoss1_Phase2::MeleeAttackStart()
{
	State = EBoss1_State::Aiming;
	PatternState = EBoss1_Pattern_State::MeleeAttack;
	GetMesh()->GetAnimInstance()->Montage_Play(MeleeAttackMontages[FMath::RandRange(0, MeleeAttackMontages.Num() - 1)]);
}

void ABoss1_Phase2::MeleeAttack()
{
	State = EBoss1_State::Casting;
	CanDamageMeleeAttack = true;
}

void ABoss1_Phase2::MeleeAttackEnd()
{
	CanDamageMeleeAttack = false;
}

void ABoss1_Phase2::MeleeAttackDelayEnd()
{
	EndPattern();
}

void ABoss1_Phase2::SetToRage()
{
	// Set Rage Options
	
}
