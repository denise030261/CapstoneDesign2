// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/MonsterBase.h"

#include "Camera/CameraComponent.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "CapstoneDesign2/Talisman/FireAttribute.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monsters/MonsterUI.h"

// Sets default values
AMonsterBase::AMonsterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 Yaw 회전
	GetCharacterMovement()->bUseControllerDesiredRotation = false; // 컨트롤러의 회전 반영 비활성화
	
	MonsterUI = CreateDefaultSubobject<UWidgetComponent>(FName("MonsterUI"));
	static ConstructorHelpers::FClassFinder<UUserWidget> UIBlueprint(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/CapstoneDesign/Blueprints/Monster/BP_MonsterUI.BP_MonsterUI_C'"));
	if (UIBlueprint.Succeeded()) MonsterUI->SetWidgetClass(UIBlueprint.Class);
	MonsterUI->SetWidgetSpace(EWidgetSpace::World);
	MonsterUI->SetBlendMode(EWidgetBlendMode::Transparent);
	MonsterUI->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultController();

	NowHp = MaxHp;
	
	if (GetWorld()->GetFirstPlayerController())
		PlayerCharacter = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	Cast<UMonsterUI>(MonsterUI->GetUserWidgetObject())->SetMonsterName(Name);
	Cast<UMonsterUI>(MonsterUI->GetUserWidgetObject())->SetMaxHp(MaxHp);
	Cast<UMonsterUI>(MonsterUI->GetUserWidgetObject())->SetHp(NowHp);
}

// Called every frame
void AMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAnimInstance();
	UpdateUI();
}

void AMonsterBase::DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset)
{
	NowHp = FMath::Clamp(NowHp - DamageAmount, 0.0f, MaxHp);
	Cast<UMonsterUI>(MonsterUI->GetUserWidgetObject())->SetHp(NowHp);
		
	if (NowHp <= 0)
	{
		SetDie();
	}
}

void AMonsterBase::UpdateUI() const
{
	const FVector CameraLocation = PlayerCharacter->GetFollowCamera()->GetComponentLocation();
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MonsterUI->GetComponentLocation(), CameraLocation);

	MonsterUI->SetWorldRotation(LookAtRotation);
}

void AMonsterBase::UpdateAnimInstance() const
{
}

void AMonsterBase::SetDie()
{
}

float AMonsterBase::CalcDistance() const
{
	const FVector StartPos = GetActorLocation();
	const FVector EndPos = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	return (StartPos - EndPos).Size();
}

FRotator AMonsterBase::CalcSmoothLookAtRotation(const FVector& Location, const float DeltaTime) const
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;
	const FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);

	return SmoothRotation;
}

void AMonsterBase::Gaze(const float DeltaTime)
{
	const FRotator SmoothRotation = CalcSmoothLookAtRotation(PlayerCharacter->GetActorLocation(), DeltaTime);

	SetActorRotation(SmoothRotation);
}