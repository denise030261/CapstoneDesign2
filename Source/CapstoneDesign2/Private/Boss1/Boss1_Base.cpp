// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_Base.h"

#include "Boss1/Boss1Anim.h"
#include "Boss1/Boss1_IronGenerator.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

class UBoss1Anim;
// Sets default values
ABoss1_Base::ABoss1_Base()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 Yaw 회전
	GetCharacterMovement()->bUseControllerDesiredRotation = false; // 컨트롤러의 회전 반영 비활성화
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(120.0f);
	GetCapsuleComponent()->SetCapsuleRadius(60.0f);
	GetCapsuleComponent()->SetRelativeScale3D(FVector(1.0f));
	GetCapsuleComponent()->SetCollisionProfileName(FName("Boss1"));
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ABoss1_Base::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABoss1_Base::OnOverlapBegin);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonGrux/Characters/Heroes/Grux/Skins/Tier_2/Grux_Beetle_Molten/Meshes/GruxMolten.GruxMolten'"));
	if (MeshAsset.Succeeded()) GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -110.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	//Animation
	//Trouble Shooting: 블루프린트 가져올때는 경로 끝에 _C 꼭 붙이기
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBlueprint(TEXT("/Script/Engine.AnimBlueprint'/Game/CapstoneDesign/Blueprints/Boss/Boss1/ABP_Boss1Anim.ABP_Boss1Anim_C'"));
	if (AnimBlueprint.Succeeded()) GetMesh()->SetAnimInstanceClass(AnimBlueprint.Class);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimPatternMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Boss/Boss1/AM_Boss1_Pattern.AM_Boss1_Pattern'"));
	if (AnimPatternMontage.Succeeded()) PatternMontage = AnimPatternMontage.Object;
}

// Called when the game starts or when spawned
void ABoss1_Base::BeginPlay()
{
	Super::BeginPlay();

	SpawnDefaultController();
	
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnShootNeedleAimingEndNotify.AddDynamic(this, &ABoss1_Base::ShootNeedle);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnThrowMassAimingEndNotify.AddDynamic(this, &ABoss1_Base::ThrowMass);
	Cast<UBoss1Anim>(GetMesh()->GetAnimInstance())->OnEndOnceNotify.AddDynamic(this, &ABoss1_Base::ShootNeedleEnd);
}

// Called every frame
void ABoss1_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckState(DeltaTime);
}

void ABoss1_Base::SetStateIdle()
{
	State = EBoss1_State::Idle;
	GetWorldTimerManager().SetTimer(IdleTimerHandle, [&]
	{
		SetTargetIron();
		State = EBoss1_State::Tracing;
	}, IdleSecond, false);
}

void ABoss1_Base::SetTargetIron()
{
	float MinLength = FLT_MAX;

	for (ABoss1_Iron* Iron : IronGenerator->SpawnedIrons)
	{
		if (IsValid(Iron))
		{
			const float Length = FVector::DistSquared(GetActorLocation(), Iron->GetActorLocation());
			if (Length < MinLength)
			{
				TargetIron = Iron;
				MinLength = Length;
			}
		}
	}
}

void ABoss1_Base::EatIron(ABoss1_Iron* Iron)
{
	State = EBoss1_State::Eating;
	InitGrow(GetActorRelativeScale3D().X, GetActorRelativeScale3D().X * EatIronScaleFactor, EatIronTime);
	GetWorldTimerManager().SetTimer(CastingTimerHandle, [&] { EndEatIron(); }, EatIronTime, false);
	
	IronGenerator->RemoveIron(Iron);
	if (NowIronCount < MaxIronCount)
	{
		NowIronCount++;
		MoveSpeed *= 1.1f;
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
	else
	{
		NowHp = FMath::Clamp(NowHp + EatIronHealValue, 0.0f, MaxHp);
	}
}

FRotator ABoss1_Base::CalcSmoothLookAtRotation(const FVector& Location, const float DeltaTime) const
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;
	const FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);

	return SmoothRotation;
}

void ABoss1_Base::Aiming(float DeltaTime)
{
	const FRotator SmoothRotation = CalcSmoothLookAtRotation(PlayerCharacter->GetActorLocation(), DeltaTime);

	SetActorRotation(SmoothRotation);
}

void ABoss1_Base::MoveToIron(float DeltaTime)
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

void ABoss1_Base::ShootNeedleStart()
{
	State = EBoss1_State::Aiming;
	PatternState = EBoss1_Pattern_State::ShootNeedle;
	GetMesh()->GetAnimInstance()->Montage_Play(PatternMontage);
}

void ABoss1_Base::ShootNeedleEnd()
{
	if (PatternState == EBoss1_Pattern_State::ShootNeedle)
	{
		EndPattern();
	}
}

void ABoss1_Base::ThrowMassStart()
{
	State = EBoss1_State::Aiming;
	PatternState = EBoss1_Pattern_State::ThrowMass;
	GetMesh()->GetAnimInstance()->Montage_Play(PatternMontage);
}

void ABoss1_Base::ThrowMass()
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

void ABoss1_Base::ThrowMassEnd()
{
	if (PatternState == EBoss1_Pattern_State::ThrowMass)
	{
		EndPattern();
	}
}

void ABoss1_Base::EndPattern()
{
	PatternState = EBoss1_Pattern_State::None;
	GetMesh()->GetAnimInstance()->Montage_Stop(0.0f);
	IdleSecond = FMath::FRand() * IdleSecondBase;
	SetStateIdle();
}

void ABoss1_Base::InitGrow(const float StartScale, const float EndScale, const float TotalTime)
{
	GrowStartScale = StartScale;
	GrowEndScale = EndScale;
	GrowDeltaTime = 0.0f;
	GrowTotalTime = TotalTime;
}

void ABoss1_Base::Grow(float DeltaTime)
{
	GrowDeltaTime += DeltaTime;
	const float Alpha = FMath::Clamp(GrowDeltaTime / GrowTotalTime, 0.0f, 1.0f);
	const float NewScale = FMath::Lerp(GrowStartScale, GrowEndScale, Alpha);
	SetActorRelativeScale3D(FVector(NewScale));
}