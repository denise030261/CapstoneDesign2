// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1.h"

#include "Boss1/Boss1Anim.h"
#include "Boss1/Boss1_Iron.h"
#include "Boss1/Boss1_IronGenerator.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
ABoss1::ABoss1()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 Yaw 회전
	GetCharacterMovement()->bUseControllerDesiredRotation = false; // 컨트롤러의 회전 반영 비활성화
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeedBase1;
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(110.0f);
	GetCapsuleComponent()->SetCapsuleRadius(60.0f);
	GetCapsuleComponent()->SetCollisionProfileName(FName("Boss1"));
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABoss1::OnOverlapBegin);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Phase1MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonGrux/Characters/Heroes/Grux/Meshes/Grux.Grux'"));
	if (Phase1MeshAsset.Succeeded()) Phase1Mesh = Phase1MeshAsset.Object;
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Phase2MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonGrux/Characters/Heroes/Grux/Skins/Tier_2/Grux_Beetle_Molten/Meshes/GruxMolten.GruxMolten'"));
	if (Phase2MeshAsset.Succeeded()) Phase2Mesh = Phase2MeshAsset.Object;
	
	GetMesh()->SetSkeletalMesh(Phase1Mesh);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -110.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	
	WeaponColliderL = CreateDefaultSubobject<UBoxComponent>(FName("WeaponCollider_L"));
	WeaponColliderL->OnComponentBeginOverlap.AddDynamic(this, &ABoss1::OnOverlapBegin_Weapon);
	WeaponColliderL->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));
	WeaponColliderL->SetupAttachment(GetMesh());

	WeaponColliderR = CreateDefaultSubobject<UBoxComponent>(FName("WeaponCollider_R"));
	WeaponColliderR->OnComponentBeginOverlap.AddDynamic(this, &ABoss1::OnOverlapBegin_Weapon);
	WeaponColliderR->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));
	WeaponColliderR->SetupAttachment(GetMesh());

	ShieldParticleComp = CreateDefaultSubobject<UNiagaraComponent>(FName("Shield Particle"));
	ShieldParticleComp->bAutoActivate = false;
	ShieldParticleComp->SetAbsolute(false, true, false);
	ShieldParticleComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ShieldParticleAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/MegaMagicVFXBundle/VFX/MagicShieldsVFX/VFX/DefaultVersions/MagmaShield/Systems/N_MagmaShield.N_MagmaShield'"));
	if (ShieldParticleAsset.Succeeded()) ShieldParticleComp->SetAsset(ShieldParticleAsset.Object);

	Phase3ParticleComp = CreateDefaultSubobject<UNiagaraComponent>(FName("Phase3 Particle"));
	Phase3ParticleComp->bAutoActivate = false;
	Phase3ParticleComp->SetAbsolute(false, true, false);
	Phase3ParticleComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> Phase3ParticleAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/MegaMagicVFXBundle/VFX/MagicAuraVFX/VFX/Tsunami/Systems/N_BossEffect.N_BossEffect'"));
	if (Phase3ParticleAsset.Succeeded()) Phase3ParticleComp->SetAsset(Phase3ParticleAsset.Object);
	
	//Animation
	//Trouble Shooting: 블루프린트 가져올때는 경로 끝에 _C 꼭 붙이기
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBlueprint(TEXT("/Script/Engine.AnimBlueprint'/Game/CapstoneDesign/Blueprints/Boss/Boss1/ABP_Boss1Anim.ABP_Boss1Anim_C'"));
	if (AnimBlueprint.Succeeded()) GetMesh()->SetAnimInstanceClass(AnimBlueprint.Class);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimSpawnMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Boss/Boss1/AM_Boss1_Spawn.AM_Boss1_Spawn'"));
	if (AnimSpawnMontage.Succeeded()) SpawnMontage = AnimSpawnMontage.Object;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimPatternMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Boss/Boss1/AM_Boss1_Pattern.AM_Boss1_Pattern'"));
	if (AnimPatternMontage.Succeeded()) PatternMontage = AnimPatternMontage.Object;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimMeleeAttackMontageL(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Boss/Boss1/AM_Boss1_MeleeAttack_L.AM_Boss1_MeleeAttack_L'"));
	if (AnimMeleeAttackMontageL.Succeeded()) MeleeAttackMontages.Add(AnimMeleeAttackMontageL.Object);
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimMeleeAttackMontageR(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Boss/Boss1/AM_Boss1_MeleeAttack_R.AM_Boss1_MeleeAttack_R'"));
	if (AnimMeleeAttackMontageR.Succeeded()) MeleeAttackMontages.Add(AnimMeleeAttackMontageR.Object);
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimDieMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Boss/Boss1/AM_Boss1_Die.AM_Boss1_Die'"));
	if (AnimDieMontage.Succeeded()) DieMontage = AnimDieMontage.Object;
}

// Called when the game starts or when spawned
void ABoss1::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultController();

	NowHp = MaxHp;
	
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnShootNeedleAimingEndNotify.AddDynamic(this, &ABoss1::ShootNeedle);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnThrowMassAimingEndNotify.AddDynamic(this, &ABoss1::ThrowMass);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnEndOnceNotify.AddDynamic(this, &ABoss1::ShootNeedleEnd);

	SetPhase1();
}

// Called every frame
void ABoss1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckState(DeltaTime);
	
	if (Phase == 1 && IsActivate)
	{
		Phase1RemainSecond -= DeltaTime;
	}
	else
	{
		if (IsActivate)
		{
			HealRemainSecond -= DeltaTime;
		}
		UpdateWeaponCollider();
	}
}

void ABoss1::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (ABoss1_Iron* Iron = Cast<ABoss1_Iron>(OtherActor))
		{
			if (Phase == 1 || (Phase == 2 && IsHealPattern))
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

void ABoss1::OnOverlapBegin_Weapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanDamageMeleeAttack && OtherActor && OtherActor != this)
	{
		if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
		{
			Player->SetCharacterHP(-MeleeAttackDamage * FMath::Pow(EatIronDamageFactor, NowIronCount));
			CanDamageMeleeAttack = false;
		}
	}
}
	
void ABoss1::DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset)
{
	if (FName("FireAttribute") == DataAsset->SkillInfo.Attribute->GetName() && (State != EBoss1_State::Spawn || State != EBoss1_State::Die))
	{
		if (Phase == 1 || IsHealPattern)
		{
			if (State == EBoss1_State::Tracing)
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
			NowHp = FMath::Clamp(NowHp - DamageAmount, 0.0f, MaxHp);

			if (Phase == 2)
			{
				if (HealStartHpsIndex < HealStartHps.Num() && NowHp <= HealStartHps[HealStartHpsIndex])
				{
					HealStartHpsIndex++;
					HealStart();
				}
				else if (NowHp <= Phase3StartHp)
				{
					SetPhase3();
				}
			}
			else // if (Phase == 3)
			{
				if (NowHp <= 0.0f)
				{
					SetDie();
				}
			}
		}
	}
	else
	{
	}
}

void ABoss1::CheckState(float DeltaTime)
{
	if (Phase == 1 || IsHealPattern)
	{
		switch (State)
		{
		case EBoss1_State::Eating:
			Grow(DeltaTime);
			break;
			
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
	else
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
}

void ABoss1::SetStateIdle()
{
	State = EBoss1_State::Idle;
	GetWorldTimerManager().SetTimer(IdleTimerHandle, [&]
	{
		SetTargetIron();
		State = EBoss1_State::Tracing;
	}, IdleSecond, false);
}

void ABoss1::SetTargetIron()
{
	float MinLength = FLT_MAX;

	for (ABoss1_Iron* Iron : IronGenerator->SpawnedIrons)
	{
		if (IsValid(Iron))
		{
			if (const float Length = FVector::DistSquared(GetActorLocation(), Iron->GetActorLocation()); Length < MinLength)
			{
				TargetIron = Iron;
				MinLength = Length;
			}
		}
	}
}

void ABoss1::EatIron(ABoss1_Iron* Iron)
{
	State = EBoss1_State::Eating;
	if (Phase == 1)
	{
		InitGrow(GetActorRelativeScale3D().X, GetActorRelativeScale3D().X * EatIronScaleFactor, EatIronTime);
	}
	GetWorldTimerManager().SetTimer(CastingTimerHandle, this, &ABoss1::EndEatIron, EatIronTime, false);
	
	IronGenerator->RemoveIron(Iron);
	if (NowIronCount < MaxIronCount)
	{
		NowIronCount++;
		if (Phase == 1)
		{
			GetCharacterMovement()->MaxWalkSpeed = MoveSpeedBase1 * FMath::Pow(1.1f, NowIronCount);
		}
	}
	else
	{
		NowHp = FMath::Clamp(NowHp + EatIronHealValue, 0.0f, MaxHp);
	}
}

void ABoss1::EndEatIron()
{
	IdleSecond = IdleSecondBase;
	SetStateIdle();
	
	if (Phase == 1 && NowIronCount >= MaxIronCount)
	{
		SetPhase2();
	}
}

void ABoss1::Trace(float DeltaTime)
{
	if (Phase == 1)
	{
		MoveToIron(DeltaTime);
	}
	else
	{
		const float Prob_Tick = 1 - FMath::Pow(1 - (ShootNeedleProb + ThrowMassProb + MeleeAttackProb), DeltaTime);

		if (FMath::FRand() < Prob_Tick)
		{
			if (Phase == 2)
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
			else // if (Phase == 3)
			{
				MeleeAttackStart();
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
			else
			{
				MeleeAttackStart();
			}
		}
	}
}

void ABoss1::Aiming(float DeltaTime)
{
	const FRotator SmoothRotation = CalcSmoothLookAtRotation(PlayerCharacter->GetActorLocation(), DeltaTime);
	SetActorRotation(SmoothRotation);
}

void ABoss1::MoveToIron(float DeltaTime)
{
	if (TargetIron)
	{
		const FRotator SmoothRotation = CalcSmoothLookAtRotation(TargetIron->GetActorLocation(), DeltaTime);
		
		if (SmoothRotation.Equals(GetActorRotation(), 0.1f))  // 0.1도 차이면 멈춤
		{
			AddMovementInput(TargetIron->GetActorLocation() - GetActorLocation());
		}
		else
		{
			SetActorRotation(SmoothRotation);
		}
	}
	else
	{
		SetTargetIron();
	}
}

void ABoss1::ShootNeedleStart()
{
	State = EBoss1_State::Aiming;
	PatternState = EBoss1_Pattern_State::ShootNeedle;
	GetMesh()->GetAnimInstance()->Montage_Play(PatternMontage);
}

void ABoss1::ShootNeedle()
{
	if (PatternState == EBoss1_Pattern_State::ShootNeedle)
	{
		State = EBoss1_State::Casting;
		const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerCharacter->GetActorLocation());
		TArray Rots = Phase == 1 ? TArray({ 0.0f }) : TArray({ -15.0f, -30.0f, 0.0f, 15.0f, 30.0f });
		
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

void ABoss1::ShootNeedleEnd()
{
	if (PatternState == EBoss1_Pattern_State::ShootNeedle)
	{
		EndPattern();
	}
}

void ABoss1::ThrowMassStart()
{
	State = EBoss1_State::Aiming;
	PatternState = EBoss1_Pattern_State::ThrowMass;
	GetMesh()->GetAnimInstance()->Montage_Play(PatternMontage);
}

void ABoss1::ThrowMass()
{
	if (PatternState == EBoss1_Pattern_State::ThrowMass)
	{
		State = EBoss1_State::Casting;

		const float Speed = 600.0f + 0.8f * FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
	
		UGameplayStatics::FSuggestProjectileVelocityParameters ProjectileParams = UGameplayStatics::FSuggestProjectileVelocityParameters(this, GetActorLocation(), PlayerCharacter->GetActorLocation(), Speed);
		ProjectileParams.ActorsToIgnore.Append({ this, PlayerCharacter });
		ProjectileParams.bFavorHighArc = false;

		FVector LaunchVelocity;
		if (!UGameplayStatics::SuggestProjectileVelocity(ProjectileParams, LaunchVelocity))
			LaunchVelocity = PlayerCharacter->GetActorLocation() - GetActorLocation();
		
		ABoss1_Projectile_Mass* Mass = GetWorld()->SpawnActor<ABoss1_Projectile_Mass>(MassProjectile, GetActorLocation(), LaunchVelocity.Rotation());
		Mass->Damage = ThrowMassDamage * FMath::Pow(EatIronDamageFactor, NowIronCount);
		Mass->SetActorRelativeScale3D(Mass->GetActorRelativeScale3D() * FMath::Pow(EatIronScaleFactor, NowIronCount));
		Mass->ProjectileMovement->Velocity = LaunchVelocity;
		Mass->ProjectileMovement->InitialSpeed = Speed;
		Mass->ProjectileMovement->MaxSpeed = Speed;

		#if WITH_EDITOR
		Mass->SetFolderPath(FName("Projectiles"));
		#endif
	}
}

void ABoss1::ThrowMassEnd()
{
	if (PatternState == EBoss1_Pattern_State::ThrowMass)
	{
		EndPattern();
	}
}

void ABoss1::EndPattern()
{
	PatternState = EBoss1_Pattern_State::None;
	GetMesh()->GetAnimInstance()->Montage_Stop(0.0f);
	IdleSecond = (FMath::FRand() + 2.0f) * IdleSecondBase;
	SetStateIdle();
}

void ABoss1::UpdateWeaponCollider() const
{
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

	#if WITH_EDITOR
	const FColor Color = CanDamageMeleeAttack ? FColor::Red : FColor::Green;
	
	DrawDebugBox(GetWorld(),
		WeaponColliderL->GetComponentLocation(),
		WeaponColliderL->GetScaledBoxExtent(),
		WeaponColliderL->GetComponentQuat(),
		Color, false, -1.f, 0, 2.f);

	DrawDebugBox(GetWorld(),
		WeaponColliderR->GetComponentLocation(),
		WeaponColliderR->GetScaledBoxExtent(),
		WeaponColliderR->GetComponentQuat(),
		Color, false, -1.f, 0, 2.f);
	#endif
}

void ABoss1::MeleeAttackStart()
{
	State = EBoss1_State::Aiming;
	PatternState = EBoss1_Pattern_State::MeleeAttack;
	GetMesh()->GetAnimInstance()->Montage_Play(MeleeAttackMontages[FMath::RandRange(0, MeleeAttackMontages.Num() - 1)]);
}

void ABoss1::MeleeAttack()
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

void ABoss1::MeleeAttackEnd()
{
	CanDamageMeleeAttack = false;
}

void ABoss1::MeleeAttackDelayEnd()
{
	EndPattern();
}

void ABoss1::HealStart()
{
	State = EBoss1_State::Spawn;
	IsHealPattern = true;
	GetWorldTimerManager().ClearTimer(IdleTimerHandle);
	GetWorldTimerManager().ClearTimer(CastingTimerHandle);
	GetMesh()->GetAnimInstance()->Montage_Stop(0);
	ShieldParticleComp->Activate();
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeedBase1 * FMath::Pow(1.1f, MaxIronCount);
	HealRemainSecond = HealSecond;

	FTimerHandle HealHandle;
	GetWorldTimerManager().SetTimer(HealHandle, [&]
	{
		SetStateIdle();
	}
	, 2.0f, false);
	
	GetWorldTimerManager().SetTimer(HealTimerHandle, [&]
	{
		IsHealPattern = false;
		IdleSecond = IdleSecondBase;
		SetStateIdle();
		ShieldParticleComp->DeactivateImmediate();
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeedBase2;
	}
	, HealSecond, false);
}

void ABoss1::SetPhase1()
{
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnEndOnceNotify.AddDynamic(this, &ABoss1::ThrowMassEnd);

	PlayerCharacter = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	IronGenerator = GetWorld()->SpawnActor<ABoss1_IronGenerator>(IronGenerator->StaticClass(), GetActorLocation(), GetActorRotation());
	IdleSecond = IdleSecondBase;
	IsActivate = true;
	
	GetWorldTimerManager().SetTimer(Phase1TimerHandle, this, &ABoss1::SetPhase2, Phase1Second, false);

	FTimerHandle SpawnHandle;
	GetWorldTimerManager().SetTimer(SpawnHandle, this, &ABoss1::SetStateIdle, 2.0f, false);
	
	GetMesh()->GetAnimInstance()->Montage_Play(SpawnMontage);
}

void ABoss1::SetPhase2()
{
	GetWorldTimerManager().ClearTimer(Phase1TimerHandle);
	GetWorldTimerManager().ClearTimer(IdleTimerHandle);
	GetWorldTimerManager().ClearTimer(CastingTimerHandle);
	GetMesh()->GetAnimInstance()->Montage_Stop(0);

	Phase = 2;
	State = EBoss1_State::Spawn;
	PatternState = EBoss1_Pattern_State::None;
	IdleSecond = IdleSecondBase;
	
	GetMesh()->SetSkeletalMesh(Phase2Mesh);
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeedBase2;
	
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnEndOnceNotify.RemoveDynamic(this, &ABoss1::ThrowMassEnd);
	
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnEndAllNotify.AddDynamic(this, &ABoss1::ThrowMassEnd);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnMeleeAttackStartNotify.AddDynamic(this, &ABoss1::MeleeAttack);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnMeleeAttackEndNotify.AddDynamic(this, &ABoss1::MeleeAttackEnd);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnMeleeAttackDelayEndNotify.AddDynamic(this, &ABoss1::MeleeAttackDelayEnd);

	FTimerHandle SpawnHandle;
	constexpr float SpawnTime = 3.0f;
	InitGrow(GetActorRelativeScale3D().X, FMath::Pow(EatIronScaleFactor, MaxIronCount), SpawnTime);
	GetWorldTimerManager().SetTimer(SpawnHandle, this, &ABoss1::SetStateIdle, SpawnTime, false);
}

void ABoss1::SetPhase3()
{
	Phase = 3;
	State = EBoss1_State::Spawn;
	PatternState = EBoss1_Pattern_State::None;
	NowIronCount = MaxIronCount;
	IdleSecond = IdleSecondBase;
	
	GetWorldTimerManager().ClearTimer(IdleTimerHandle);
	GetWorldTimerManager().ClearTimer(CastingTimerHandle);
	GetMesh()->GetAnimInstance()->Montage_Stop(0);
	
	FTimerHandle SetRageHandle;
	constexpr float GrowTime = 3.0f;
	InitGrow(GetActorRelativeScale3D().X, GetActorRelativeScale3D().X * 1.5f, GrowTime);
	GetWorldTimerManager().SetTimer(SetRageHandle, [&]
	{
		SetStateIdle();
	},
	GrowTime,
	false);
}

void ABoss1::SetDie()
{
	State = EBoss1_State::Die;
	PatternState = EBoss1_Pattern_State::None;

	GetWorldTimerManager().ClearTimer(IdleTimerHandle);
	GetWorldTimerManager().ClearTimer(CastingTimerHandle);
	GetWorldTimerManager().ClearAllTimersForObject(IronGenerator);
	GetCharacterMovement()->DisableMovement();
	GetMesh()->GetAnimInstance()->Montage_Play(DieMontage);
}

void ABoss1::InitGrow(const float StartScale, const float EndScale, const float TotalTime)
{
	GrowStartScale = StartScale;
	GrowEndScale = EndScale;
	GrowDeltaTime = 0.0f;
	GrowTotalTime = TotalTime;
}

void ABoss1::Grow(float DeltaTime)
{
	GrowDeltaTime += DeltaTime;
	const float Alpha = FMath::Clamp(GrowDeltaTime / GrowTotalTime, 0.0f, 1.0f);
	const float NewScale = FMath::Lerp(GrowStartScale, GrowEndScale, Alpha);
	SetActorRelativeScale3D(FVector(NewScale));
}

FRotator ABoss1::CalcSmoothLookAtRotation(const FVector& Location, float DeltaTime) const
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;
	const FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);

	return SmoothRotation;
}
