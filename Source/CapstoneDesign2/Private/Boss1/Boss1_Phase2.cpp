// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_Phase2.h"

#include "Boss1/Boss1Anim.h"
#include "Boss1/Boss1_Iron.h"
#include "Boss1/Boss1_IronGenerator.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "CapstoneDesign2/Talisman/FireAttribute.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"

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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimDieMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Boss/Boss1/AM_Boss1_Die.AM_Boss1_Die'"));
	if (AnimDieMontage.Succeeded()) DieMontage = AnimDieMontage.Object;
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
	constexpr float SpawnTime = 3.0f;
	InitGrow(GetActorRelativeScale3D().X, FMath::Pow(EatIronScaleFactor, MaxIronCount), SpawnTime);
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
			if (PatternState == EBoss1_Pattern_State::Heal)
			{
				EatIron(Iron);
			}
			else
			{
				IronGenerator->RemoveIron(Iron);
			}
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
				Player->SetCharacterHP(-MeleeAttackDamage * FMath::Pow(EatIronDamageFactor, NowIronCount));
				CanDamageMeleeAttack = false;
			}
		}
	}
}
void ABoss1_Phase2::CheckState(float DeltaTime)
{
	switch (State)
	{
	case EBoss1_State::Spawn:
		Grow(DeltaTime);
		break;
		
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
	const float Prob_Tick = 1 - FMath::Pow(1 - (ShootNeedleProb + ThrowMassProb + MeleeAttackProb), DeltaTime);

	if (FMath::FRand() < Prob_Tick)
	{
		if (IsRage)
		{
			MeleeAttackStart();
		}
		else
		{
			const float Rand = FMath::FRandRange(0.0f, ShootNeedleProb + ThrowMassProb + MeleeAttackProb);
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
	}
	else
	{
		const FRotator SmoothRotation = CalcSmoothLookAtRotation(PlayerCharacter->GetActorLocation(), DeltaTime);
		SetActorRotation(SmoothRotation);
		
		const FVector GroundPos = GetActorLocation() - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		const FVector PlayerGroundPos = PlayerCharacter->GetActorLocation() - FVector(0.0f, 0.0f, PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		
		if (FVector::Dist(GroundPos, PlayerGroundPos) > GetCapsuleComponent()->GetScaledCapsuleRadius() * 3.0f)
		{
			AddMovementInput(GetActorForwardVector());
		}
	}
}

void ABoss1_Phase2::EndEatIron()
{
	SetStateIdle();
}

void ABoss1_Phase2::ShootNeedle()
{
	if (PatternState == EBoss1_Pattern_State::ShootNeedle)
	{
		State = EBoss1_State::Casting;

		TArray Rots = { -15.0f, -30.0f, 0.0f, 15.0f, 30.0f };
		
		const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerCharacter->GetActorLocation());
		for (int32 i = 0; i < Rots.Num(); i++)
		{
			ABoss1_Projectile_Needle* Needle = GetWorld()->SpawnActor<ABoss1_Projectile_Needle>(NeedleProjectile, GetActorLocation(), LookAt + FRotator(0.0f, Rots[i], 0.0f));
			Needle->Damage = ShootNeeleDamage * FMath::Pow(EatIronDamageFactor, NowIronCount);
			Needle->SetActorRelativeScale3D(Needle->GetActorRelativeScale3D() * FMath::Pow(EatIronScaleFactor, NowIronCount));
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
	
	FRotator Rotation = GetActorRotation();
	FVector StartLocation = GetActorLocation();
	StartLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector Direction = PlayerCharacter->GetActorLocation() - StartLocation;
	Direction.Z = 0;
	Direction.Normalize();
	int32 i = 1;
	
	FTimerHandle ShockHandle;
	GetWorldTimerManager().SetTimer(ShockHandle, [&, Rotation, StartLocation, Direction, i, this]() mutable
	{
		if (i > 10)
		{
			GetWorldTimerManager().ClearTimer(ShockHandle);
			return;
		}

		const FVector Location = StartLocation + Direction * 500.0f * i;
		ABoss1_MeleeAttackShock* Shock = GetWorld()->SpawnActor<ABoss1_MeleeAttackShock>(MeleeAttackShock, Location, Rotation);
		Shock->SetActorRelativeScale3D(Shock->GetActorRelativeScale3D() * FMath::Pow(EatIronScaleFactor, NowIronCount));
		Shock->Damage = MeleeAttackShockDamage * FMath::Pow(EatIronDamageFactor, NowIronCount);
		DrawDebugSphere(GetWorld(), Shock->GetActorLocation(), Shock->CollisionComponent->GetScaledSphereRadius(), 12, FColor::Red, false, 1.5f);
		i++;
	},
	0.2f,
	true,
	0.6f
	);
}

void ABoss1_Phase2::MeleeAttackEnd()
{
	CanDamageMeleeAttack = false;
}

void ABoss1_Phase2::MeleeAttackDelayEnd()
{
	EndPattern();
}

void ABoss1_Phase2::SetRage()
{
	IsRage = true;
	State = EBoss1_State::Spawn;
	
	GetWorldTimerManager().ClearAllTimersForObject(this);
	GetMesh()->GetAnimInstance()->Montage_Stop(0);
	
	FTimerHandle SetRageHandle;
	constexpr float GrowTime = 3.0f;
	InitGrow(GetActorRelativeScale3D().X, GetActorRelativeScale3D().X * 1.5f, GrowTime);
	GetWorldTimerManager().SetTimer(SetRageHandle, [&]
	{
		// Need Fix
		/*
		FTimerHandle ShockHandle;
		GetWorldTimerManager().SetTimer(ShockHandle, [&]
		{
			FVector SpawnPos = GetActorLocation();
			SpawnPos.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			auto Shock = GetWorld()->SpawnActor<ABoss1_RageShock>(RageShock, SpawnPos, GetActorRotation());
			DrawDebugSphere(GetWorld(), Shock->GetActorLocation(), Shock->CollisionComponent->GetScaledSphereRadius(), 12, FColor::Red, false, 1.5f);
		
			UE_LOG(LogTemp, Warning, TEXT("BOOM!"))
		},
		1.0f,
		true);
		*/
		
		SetStateIdle();
	},
	GrowTime,
	false);
}

void ABoss1_Phase2::SetDie()
{
	State = EBoss1_State::Die;

	GetWorldTimerManager().ClearAllTimersForObject(this);
	GetWorldTimerManager().ClearAllTimersForObject(IronGenerator);
	GetCharacterMovement()->DisableMovement();
	GetMesh()->GetAnimInstance()->Montage_Play(DieMontage);
}

void ABoss1_Phase2::DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset)
{
	if (FName("FireAttribute") == DataAsset->SkillInfo.Attribute->GetName())
	{
		UE_LOG(LogTemp, Warning, TEXT("불 히트!"));

		NowHp = FMath::Clamp(NowHp - DamageAmount, 0.0f, MaxHp);

		if (NowHp <= RageStartHp)
		{
			SetRage();
		}

		if (NowHp <= 0.0f)
		{
			SetDie();
		}
	}
	else
	{
		
	}
}

