// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/Monster1/Monster1.h"

#include "Camera/CameraComponent.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "CapstoneDesign2/Talisman/FireAttribute.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monsters/MonsterUI.h"
#include "Monsters/Monster1/Monster1Anim.h"
#include "Sound/SoundCue.h"

// Sets default values
AMonster1::AMonster1()
{
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(82.0f);
	GetCapsuleComponent()->SetCapsuleRadius(20.0f);
	GetCapsuleComponent()->SetRelativeScale3D(FVector(1.2f));
	GetCapsuleComponent()->SetCollisionProfileName(FName("Monster1"));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ROG_Creatures/Stickman/Meshes/SK_Stickman.SK_Stickman'"));
	if (MeshAsset.Succeeded()) GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -82.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	MonsterUI->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	
	WeaponColliderR = CreateDefaultSubobject<USphereComponent>(FName("Weapon Collider R"));
	WeaponColliderR->OnComponentBeginOverlap.AddDynamic(this, &AMonster1::OnWeaponOverlapBegin);
	WeaponColliderR->SetSphereRadius(20.0f);
	WeaponColliderR->SetupAttachment(RootComponent);
	
	//Animation
	//Trouble Shooting: 블루프린트 가져올때는 경로 끝에 _C 꼭 붙이기
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBlueprint(TEXT("/Script/Engine.AnimBlueprint'/Game/CapstoneDesign/Blueprints/Monster/Monster1/ABP_Monster1Anim.ABP_Monster1Anim_C'"));
	if (AnimBlueprint.Succeeded()) GetMesh()->SetAnimInstanceClass(AnimBlueprint.Class);
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Monster/Monster1/AM_Monster1_Attack.AM_Monster1_Attack'"));
	if (AnimAttackMontage.Succeeded()) AttackMontage = AnimAttackMontage.Object;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimDieMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Monster/Monster1/AM_Monster1_Die.AM_Monster1_Die'"));
	if (AnimDieMontage.Succeeded()) DieMontage = AnimDieMontage.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundCue> AttackSoundAsset(TEXT("/Script/Engine.SoundCue'/Game/SmallSoundKit/SSKCue/DoorsCue/Drs_Wood_Door_Close_01_Cue.Drs_Wood_Door_Close_01_Cue'"));
	if (AttackSoundAsset.Succeeded()) AttackSound = AttackSoundAsset.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundCue> AttackHitSoundAsset(TEXT("/Script/Engine.SoundCue'/Game/SmallSoundKit/SSKCue/DarkCue/Impact_Guts_Gore_01_Cue.Impact_Guts_Gore_01_Cue'"));
	if (AttackHitSoundAsset.Succeeded()) AttackHitSound = AttackHitSoundAsset.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundCue> DieSoundAsset(TEXT("/Script/Engine.SoundCue'/Game/SmallSoundKit/SSKCue/DoorsCue/Drs_Wood_DoubleDoor_Open_01_Cue.Drs_Wood_DoubleDoor_Open_01_Cue'"));
	if (DieSoundAsset.Succeeded()) DieSound = DieSoundAsset.Object;
	
	Name = TEXT("장화훤요의 편린");
	MaxHp = 150.0f;
}

// Called when the game starts or when spawned
void AMonster1::BeginPlay()
{
	Super::BeginPlay();
	
	AnimInstance = Cast<UMonster1Anim>(GetMesh()->GetAnimInstance());
	Cast<UMonster1Anim>(GetMesh()->GetAnimInstance())->OnAttackEndNotify.AddDynamic(this, &AMonster1::EndAttack);
	Cast<UMonster1Anim>(GetMesh()->GetAnimInstance())->OnDieEndNotify.AddDynamic(this, &AMonster1::EndDie);
}

// Called every frame
void AMonster1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckState(DeltaTime);
	UpdateWeaponColliders();
}

void AMonster1::DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset)
{
	if (State != EMonster1_State::Die)
	{
		if (FName("FireAttribute") == DataAsset->SkillInfo.Attribute->GetName())
		{
			DamageAmount *= 2.0f;
		}
	
		Super::DealDamage(DamageAmount, DataAsset);
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
			UGameplayStatics::PlaySoundAtLocation(this, AttackHitSound, SweepResult.Location);
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
	UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
}

void AMonster1::EndAttack()
{
	CanDamageAttack = false;
	State = EMonster1_State::Idle;
}


void AMonster1::UpdateWeaponColliders() const
{
	const FVector WeaponRLocation = GetMesh()->GetSocketLocation(FName("middle_01_r"));
	WeaponColliderR->SetWorldLocation(WeaponRLocation);
	
#if WITH_EDITOR
	DrawDebugSphere(GetWorld(),
		WeaponColliderR->GetComponentLocation(),
		WeaponColliderR->GetScaledSphereRadius(),
		12,
		CanDamageAttack ? FColor::Red : FColor::Green,
		false);
#endif
}

void AMonster1::UpdateAnimInstance() const
{
	AnimInstance->AnimMoveSpeed = GetCharacterMovement()->Velocity.Size();
	AnimInstance->AnimState = State;
}

void AMonster1::SetDie()
{
	State = EMonster1_State::Die;
	GetMesh()->GetAnimInstance()->Montage_Play(DieMontage);
	UGameplayStatics::PlaySoundAtLocation(this, DieSound, GetActorLocation());
	GetWorldTimerManager().ClearTimer(AttackReadyHandle);
	GetWorldTimerManager().ClearTimer(GazeHandle);
	CanDamageAttack = false;
}

void AMonster1::EndDie()
{
	if (DeadMonster)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(
			DeadMonster,
			GetActorLocation(),
			GetActorRotation(),
			SpawnParams
		);
	}

	Destroy();
}
