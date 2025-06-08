// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/Monster0/Monster0.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monsters/MonsterUI.h"
#include "Monsters/Monster0/Monster0Anim.h"
#include "Sound/SoundCue.h"

// Sets default values
AMonster0::AMonster0()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(90.0f);
	GetCapsuleComponent()->SetCapsuleRadius(20.0f);
	GetCapsuleComponent()->SetRelativeScale3D(FVector(1.2f));
	GetCapsuleComponent()->SetCollisionProfileName(FName("Monster0"));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonCountess/Characters/Heroes/Countess/Skins/Tier1/Count_Carnivale/Meshes/SM_Countess_Carnivale.SM_Countess_Carnivale'"));
	if (MeshAsset.Succeeded()) GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	MonsterUI->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	
	WeaponColliderL = CreateDefaultSubobject<UBoxComponent>(FName("Weapon Collider L"));
	WeaponColliderL->OnComponentBeginOverlap.AddDynamic(this, &AMonster0::OnWeaponOverlapBegin);
	WeaponColliderL->SetBoxExtent(FVector(5.0f, 5.0f, 45.0f));
	WeaponColliderL->SetupAttachment(RootComponent);
	
	//Animation
	//Trouble Shooting: 블루프린트 가져올때는 경로 끝에 _C 꼭 붙이기
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBlueprint(TEXT("/Script/Engine.AnimBlueprint'/Game/CapstoneDesign/Blueprints/Monster/Monster0/ABP_Monster0Anim.ABP_Monster0Anim_C'"));
	if (AnimBlueprint.Succeeded()) GetMesh()->SetAnimInstanceClass(AnimBlueprint.Class);
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Monster/Monster0/AM_Monster0_Attack.AM_Monster0_Attack'"));
	if (AnimAttackMontage.Succeeded()) AttackMontage = AnimAttackMontage.Object;
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimDieMontage(TEXT("/Script/Engine.AnimMontage'/Game/CapstoneDesign/Blueprints/Monster/Monster0/AM_Monster0_Die.AM_Monster0_Die'"));
	if (AnimDieMontage.Succeeded()) DieMontage = AnimDieMontage.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> AttackSoundAsset(TEXT("/Script/Engine.SoundCue'/Game/CapstoneDesign/Sounds/swing-whoosh-weapon_Cue.swing-whoosh-weapon_Cue'"));
	if (AttackSoundAsset.Succeeded()) AttackSound = AttackSoundAsset.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundCue> AttackHitSoundAsset(TEXT("/Script/Engine.SoundCue'/Game/SmallSoundKit/SSKCue/DarkCue/Impact_Guts_Gore_01_Cue.Impact_Guts_Gore_01_Cue'"));
	if (AttackHitSoundAsset.Succeeded()) AttackHitSound = AttackHitSoundAsset.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundCue> DieSoundAsset(TEXT("/Script/Engine.SoundCue'/Game/CapstoneDesign/Sounds/Monster0-Die.Monster0-Die'"));
	if (DieSoundAsset.Succeeded()) DieSound = DieSoundAsset.Object;
	
	Name = TEXT("요괴");
	MaxHp = 100.0f;
}


// Called when the game starts or when spawned
void AMonster0::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<UMonster0Anim>(GetMesh()->GetAnimInstance())->OnCanDamageAttackEndNotify.AddDynamic(this, &AMonster0::EndCanDamageAttack);
	Cast<UMonster0Anim>(GetMesh()->GetAnimInstance())->OnAttackEndNotify.AddDynamic(this, &AMonster0::EndAttack);
	Cast<UMonster0Anim>(GetMesh()->GetAnimInstance())->OnDieEndNotify.AddDynamic(this, &AMonster0::EndDie);
}

// Called every frame
void AMonster0::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateWeaponColliders();
	UpdateUI();
	CheckState(DeltaTime);
}

void AMonster0::DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset)
{
	if (State != EMonster0_State::Die)
	{
		Super::DealDamage(DamageAmount, DataAsset);
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
			UGameplayStatics::PlaySoundAtLocation(this, AttackHitSound, SweepResult.Location);
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
	UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
}

void AMonster0::EndCanDamageAttack()
{
	CanDamageAttack = false;
}

void AMonster0::EndAttack()
{
	State = EMonster0_State::Idle;
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

#if WITH_EDITOR
	DrawDebugBox(GetWorld(),
		WeaponColliderL->GetComponentLocation(),
		WeaponColliderL->GetScaledBoxExtent(),
		WeaponColliderL->GetComponentQuat(),
		CanDamageAttack ? FColor::Red : FColor::Green,
		false);
#endif
}

void AMonster0::SetDie()
{
	State = EMonster0_State::Die;
	GetMesh()->GetAnimInstance()->Montage_Play(DieMontage);
	UGameplayStatics::PlaySoundAtLocation(this, DieSound, GetActorLocation());
	GetWorldTimerManager().ClearTimer(AttackReadyHandle);
	CanDamageAttack = false;
}

void AMonster0::EndDie()
{
	Destroy();
}
