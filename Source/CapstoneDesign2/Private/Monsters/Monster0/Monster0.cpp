// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/Monster0/Monster0.h"

#include "CapstoneDesign2/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monsters/Monster0/Monster0Anim.h"

// Sets default values
AMonster0::AMonster0()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 Yaw 회전
	GetCharacterMovement()->bUseControllerDesiredRotation = false; // 컨트롤러의 회전 반영 비활성화
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(90.0f);
	GetCapsuleComponent()->SetCapsuleRadius(20.0f);
	GetCapsuleComponent()->SetRelativeScale3D(FVector(1.2f));
	GetCapsuleComponent()->SetCollisionProfileName(FName("Monster0"));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonCountess/Characters/Heroes/Countess/Skins/Tier1/Count_Carnivale/Meshes/SM_Countess_Carnivale.SM_Countess_Carnivale'"));
	if (MeshAsset.Succeeded()) GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	WeaponColliderL = CreateDefaultSubobject<UBoxComponent>(FName("Weapon Collider L"));
	WeaponColliderL->OnComponentBeginOverlap.AddDynamic(this, &AMonster0::OnWeaponOverlapBegin);
	WeaponColliderL->SetBoxExtent(FVector(5.0f, 5.0f, 45.0f));
	WeaponColliderL->SetupAttachment(RootComponent);
	
	WeaponColliderR = CreateDefaultSubobject<UBoxComponent>(FName("Weapon Collider R"));
	WeaponColliderR->OnComponentBeginOverlap.AddDynamic(this, &AMonster0::OnWeaponOverlapBegin);
	WeaponColliderR->SetBoxExtent(FVector(5.0f, 5.0f, 45.0f));
	WeaponColliderR->SetupAttachment(RootComponent);
	
	//Animation
	//Trouble Shooting: 블루프린트 가져올때는 경로 끝에 _C 꼭 붙이기
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBlueprint(TEXT("/Script/Engine.AnimBlueprint'/Game/CapstoneDesign/Blueprints/Monster/Monster0/ABP_Monster0Anim.ABP_Monster0Anim_C'"));
	if (AnimBlueprint.Succeeded()) GetMesh()->SetAnimInstanceClass(AnimBlueprint.Class);
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Monster/Monster0/AM_Monster0_Attack.AM_Monster0_Attack'"));
	if (AnimAttackMontage.Succeeded()) AttackMontage = AnimAttackMontage.Object;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimDieMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Monster/Monster0/AM_Monster0_Die.AM_Monster0_Die'"));
	if (AnimDieMontage.Succeeded()) DieMontage = AnimDieMontage.Object;
}


// Called when the game starts or when spawned
void AMonster0::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultController();

	NowHp = MaxHp;

	PlayerCharacter = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	Cast<UMonster0Anim>(GetMesh()->GetAnimInstance())->OnCanDamageAttackEndNotify.AddDynamic(this, &AMonster0::EndCanDamageAttack);
	Cast<UMonster0Anim>(GetMesh()->GetAnimInstance())->OnAttackEndNotify.AddDynamic(this, &AMonster0::EndAttack);
}

// Called every frame
void AMonster0::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateWeaponColliders();
	CheckState(DeltaTime);
}

void AMonster0::DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset)
{
	NowHp = FMath::Clamp(NowHp - DamageAmount, 0.0f, MaxHp);

	if (NowHp <= 0)
	{
		SetDie();
	}
}


void AMonster0::OnWeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanDamageAttack && OtherActor)
	{
		if (AMainCharacter* Character = Cast<AMainCharacter>(OtherActor))
		{
			Character->SetCharacterHP(-AttackDamage);
			CanDamageAttack = false;
		}
	}
}

void AMonster0::CheckState(const float DeltaTime)
{
	switch (State)
	{
	case EMonster0_State::Idle:
		IdleTransition(DeltaTime);
		break;
		
	default:
		break;
	}
}

void AMonster0::IdleTransition(const float DeltaTime)
{
	const FVector StartPos = GetActorLocation();
	const FVector EndPos = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	const float Distance = (StartPos - EndPos).Size();
	
	if (Distance <= GazeDistance)
	{
		Gaze(DeltaTime);
	}
	
	if (Distance <= AttackDistance)
	{
		if (!GetWorldTimerManager().IsTimerActive(AttackReadyHandle))
		{
			GetWorldTimerManager().SetTimer(AttackReadyHandle, this, &AMonster0::StartAttack, AttackReadyTime, false);
		}
	}
}

void AMonster0::StartAttack()
{
	State = EMonster0_State::Attack;

	CanDamageAttack = true;
	GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
}

void AMonster0::EndCanDamageAttack()
{
	CanDamageAttack = false;
}

void AMonster0::EndAttack()
{
	State = EMonster0_State::Idle;
}

FRotator AMonster0::CalcSmoothLookAtRotation(const FVector& Location, const float DeltaTime) const
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;
	const FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);

	return SmoothRotation;
}

void AMonster0::Gaze(float DeltaTime)
{
	const FRotator SmoothRotation = CalcSmoothLookAtRotation(PlayerCharacter->GetActorLocation(), DeltaTime);

	SetActorRotation(SmoothRotation);
}


void AMonster0::UpdateWeaponColliders() const
{
	const FVector WeaponLStartPos = GetMesh()->GetSocketLocation(FName("FX_WeaponBase_L"));
	const FVector WeaponLEndPos = GetMesh()->GetSocketLocation(FName("FX_WeaponTip_L"));
	const FVector WeaponLLocation = (WeaponLStartPos + WeaponLEndPos) / 2.0f;
	FRotator WeaponLRotation = (WeaponLEndPos - WeaponLStartPos).Rotation();
	WeaponLRotation.Pitch += 90;
	
	WeaponColliderL->SetWorldLocation(WeaponLLocation);
	WeaponColliderL->SetWorldRotation(WeaponLRotation);

	const FVector WeaponRStartPos = GetMesh()->GetSocketLocation(FName("FX_WeaponBase_R"));
	const FVector WeaponREndPos = GetMesh()->GetSocketLocation(FName("FX_WeaponTip_R"));
	const FVector WeaponRLocation = (WeaponRStartPos + WeaponREndPos) / 2.0f;
	FRotator WeaponRRotation = (WeaponREndPos - WeaponRStartPos).Rotation();
	WeaponRRotation.Pitch += 90;
	
	WeaponColliderR->SetWorldLocation(WeaponRLocation);
	WeaponColliderR->SetWorldRotation(WeaponRRotation);
	
	DrawDebugBox(GetWorld(),
		WeaponColliderL->GetComponentLocation(),
		WeaponColliderL->GetScaledBoxExtent(),
		WeaponColliderL->GetComponentQuat(),
		CanDamageAttack ? FColor::Red : FColor::Green, false, -1.f, 0, 2.f);

	DrawDebugBox(GetWorld(),
		WeaponColliderR->GetComponentLocation(),
		WeaponColliderR->GetScaledBoxExtent(),
		WeaponColliderR->GetComponentQuat(),
		CanDamageAttack ? FColor::Red : FColor::Green, false, -1.f, 0, 2.f);
}


void AMonster0::SetDie()
{
	State = EMonster0_State::Die;
	GetMesh()->GetAnimInstance()->Montage_Play(DieMontage);
}
