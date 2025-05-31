// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/Monster1/Monster1.h"

#include "Camera/CameraComponent.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "CapstoneDesign2/Talisman/FireAttribute.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monsters/MonsterUI.h"
#include "Monsters/Monster1/Monster1Anim.h"

// Sets default values
AMonster1::AMonster1()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 Yaw 회전
	GetCharacterMovement()->bUseControllerDesiredRotation = false; // 컨트롤러의 회전 반영 비활성화
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(82.0f);
	GetCapsuleComponent()->SetCapsuleRadius(20.0f);
	GetCapsuleComponent()->SetRelativeScale3D(FVector(1.2f));
	GetCapsuleComponent()->SetCollisionProfileName(FName("Monster1"));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ROG_Creatures/Stickman/Meshes/SK_Stickman.SK_Stickman'"));
	if (MeshAsset.Succeeded()) GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -82.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	MonsterUI = CreateDefaultSubobject<UWidgetComponent>(FName("MonsterUI"));
	static ConstructorHelpers::FClassFinder<UUserWidget> UIBlueprint(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/CapstoneDesign/Blueprints/Monster/BP_MonsterUI.BP_MonsterUI_C'"));
	if (UIBlueprint.Succeeded()) MonsterUI->SetWidgetClass(UIBlueprint.Class);
	MonsterUI->SetWidgetSpace(EWidgetSpace::World);
	MonsterUI->SetBlendMode(EWidgetBlendMode::Transparent);
	MonsterUI->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	MonsterUI->SetupAttachment(RootComponent);
	
	WeaponColliderR = CreateDefaultSubobject<USphereComponent>(FName("Weapon Collider R"));
	WeaponColliderR->OnComponentBeginOverlap.AddDynamic(this, &AMonster1::OnWeaponOverlapBegin);
	WeaponColliderR->SetSphereRadius(15.0f);
	WeaponColliderR->SetupAttachment(RootComponent);
	
	//Animation
	//Trouble Shooting: 블루프린트 가져올때는 경로 끝에 _C 꼭 붙이기
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBlueprint(TEXT("/Script/Engine.AnimBlueprint'/Game/CapstoneDesign/Blueprints/Monster/Monster1/ABP_Monster1Anim.ABP_Monster1Anim_C'"));
	if (AnimBlueprint.Succeeded()) GetMesh()->SetAnimInstanceClass(AnimBlueprint.Class);
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Monster/Monster1/AM_Monster1_Attack.AM_Monster1_Attack'"));
	if (AnimAttackMontage.Succeeded()) AttackMontage = AnimAttackMontage.Object;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimDieMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Monster/Monster1/AM_Monster1_Die.AM_Monster1_Die'"));
	if (AnimDieMontage.Succeeded()) DieMontage = AnimDieMontage.Object;
}

// Called when the game starts or when spawned
void AMonster1::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultController();

	NowHp = MaxHp;

	if (GetWorld()->GetFirstPlayerController())
		PlayerCharacter = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	AnimInstance = Cast<UMonster1Anim>(GetMesh()->GetAnimInstance());
	Cast<UMonster1Anim>(GetMesh()->GetAnimInstance())->OnAttackEndNotify.AddDynamic(this, &AMonster1::EndAttack);
	
	Cast<UMonsterUI>(MonsterUI->GetUserWidgetObject())->SetMonsterName(TEXT("이름뭘로하지"));
	Cast<UMonsterUI>(MonsterUI->GetUserWidgetObject())->SetMaxHp(MaxHp);
	Cast<UMonsterUI>(MonsterUI->GetUserWidgetObject())->SetHp(NowHp);
}

// Called every frame
void AMonster1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckState(DeltaTime);
	UpdateWeaponColliders();
	UpdateAnimInstance();
	UpdateUI();
}

void AMonster1::DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset)
{
	if (State != EMonster1_State::Die)
	{
		if (Cast<UFireAttribute>(DataAsset->SkillInfo.Attribute))
		{
			DamageAmount *= 1.5f;
		}
	
		NowHp = FMath::Clamp(NowHp - DamageAmount, 0.0f, MaxHp);
		if (NowHp <= 0)
		{
			SetDie();
		}
	}
}

void AMonster1::OnWeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AMonster1::CheckState(const float DeltaTime)
{
	switch (State)
	{
	case EMonster1_State::Idle:
		IdleTransition();
		break;

	case EMonster1_State::Gaze:
		GazeTransition(DeltaTime);
		break;
		
	case EMonster1_State::Trace:
		TraceTransition(DeltaTime);
		break;
		
	default:
		break;
	}
}

void AMonster1::IdleTransition()
{
	if (const float Distance = CalcDistance(); Distance <= TraceDistance)
	{
		StartGaze();
	}
}

void AMonster1::StartGaze()
{
	State = EMonster1_State::Gaze;
	GetWorldTimerManager().SetTimer(GazeHandle, [&]
	{
		State = EMonster1_State::Trace;
	},
	GazeTime,
	false);
}

void AMonster1::GazeTransition(const float DeltaTime)
{
	if (const float Distance = CalcDistance(); Distance > TraceDistance)
	{
		GetWorldTimerManager().ClearTimer(GazeHandle);
		State = EMonster1_State::Idle;
	}
	else
	{
		Gaze(DeltaTime);
	}
}

void AMonster1::TraceTransition(const float DeltaTime)
{
	if (const float Distance = CalcDistance(); Distance > TraceDistance)
	{
		State = EMonster1_State::Idle;
	}
	else
	{
		Gaze(DeltaTime);
		if (Distance > AttackDistance)
		{
			AddMovementInput(GetActorForwardVector());
		}
		else
		{
			if (!GetWorldTimerManager().IsTimerActive(AttackReadyHandle))
			{
				GetWorldTimerManager().SetTimer(AttackReadyHandle, this, &AMonster1::StartAttack, AttackReadyTime, false);
			}
		}
	}
}

void AMonster1::StartAttack()
{
	State = EMonster1_State::Attack;

	CanDamageAttack = true;
	GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
}

void AMonster1::EndAttack()
{
	CanDamageAttack = false;
	State = EMonster1_State::Idle;
}

float AMonster1::CalcDistance() const
{
	const FVector StartPos = GetActorLocation();
	const FVector EndPos = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	return (StartPos - EndPos).Size();
}

FRotator AMonster1::CalcSmoothLookAtRotation(const FVector& Location, const float DeltaTime) const
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;
	const FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);

	return SmoothRotation;
}

void AMonster1::Gaze(const float DeltaTime)
{
	const FRotator SmoothRotation = CalcSmoothLookAtRotation(PlayerCharacter->GetActorLocation(), DeltaTime);

	SetActorRotation(SmoothRotation);
}

void AMonster1::UpdateWeaponColliders() const
{
	const FVector WeaponRLocation = GetMesh()->GetSocketLocation(FName("middle_01_r"));
	WeaponColliderR->SetWorldLocation(WeaponRLocation);
	
	DrawDebugSphere(GetWorld(),
		WeaponColliderR->GetComponentLocation(),
		WeaponColliderR->GetScaledSphereRadius(),
		12,
		CanDamageAttack ? FColor::Red : FColor::Green,
		false);
}

void AMonster1::UpdateAnimInstance() const
{
	AnimInstance->AnimMoveSpeed = GetCharacterMovement()->Velocity.Size();
	AnimInstance->AnimState = State;
}

void AMonster1::UpdateUI() const
{
	const FVector CameraLocation = PlayerCharacter->GetFollowCamera()->GetComponentLocation();
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MonsterUI->GetComponentLocation(), CameraLocation);

	MonsterUI->SetWorldRotation(LookAtRotation);
}

void AMonster1::SetDie()
{
	State = EMonster1_State::Die;
	GetMesh()->GetAnimInstance()->Montage_Play(DieMontage);
}
