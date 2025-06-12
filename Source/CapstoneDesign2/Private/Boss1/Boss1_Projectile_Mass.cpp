// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_Projectile_Mass.h"

#include "Landscape.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Sound/SoundCue.h"

// Sets default values
ABoss1_Projectile_Mass::ABoss1_Projectile_Mass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// 충돌 컴포넌트 설정
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(FName("SphereCollider"));
	CollisionComponent->InitSphereRadius(100.0f);
	CollisionComponent->SetRelativeScale3D(FVector(0.3f));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABoss1_Projectile_Mass::OnBeginOverlap);
	RootComponent = CollisionComponent;

	CoreCollider = CreateDefaultSubobject<USphereComponent>(FName("CoreCollider"));
	CoreCollider->InitSphereRadius(45.0f);
	CoreCollider->OnComponentBeginOverlap.AddDynamic(this, &ABoss1_Projectile_Mass::OnCoreBeginOverlap);
	CoreCollider->SetupAttachment(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMesh"));
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	MeshComponent->SetRelativeScale3D(FVector(2.0f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (MeshAsset.Succeeded()) MeshComponent->SetStaticMesh(MeshAsset.Object);
	
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MaterialAsset(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Fab/Megascans/Surfaces/Rusty_Metal_Sheet_tj4kedvcw/Raw/MI_tj4kedvcw.MI_tj4kedvcw'"));
	if (MaterialAsset.Succeeded()) MeshComponent->SetMaterial(0, MaterialAsset.Object);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> RemovalParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonGrux/FX/Particles/Skins/Grux_Beetle_Magma/P_Grux_Magma_Ultimate_Clang.P_Grux_Magma_Ultimate_Clang'"));
	if (RemovalParticleAsset.Succeeded()) RemovalParticle = RemovalParticleAsset.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> RemovalSoundAsset(TEXT("/Script/Engine.SoundCue'/Game/CapstoneDesign/Sounds/small-rock-break-194553_Cue.small-rock-break-194553_Cue'"));
	if (RemovalSoundAsset.Succeeded()) RemovalSound = RemovalSoundAsset.Object;
	
	static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> RemovalForceFeedbackEffectAsset(TEXT("/Script/Engine.ForceFeedbackEffect'/Game/CapstoneDesign/Input/ForceFeedback/SoundForceFeedbackEffect.SoundForceFeedbackEffect'"));
	if (RemovalForceFeedbackEffectAsset.Succeeded()) RemovalForceFeedbackEffect = RemovalForceFeedbackEffectAsset.Object;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1000.0f; // 초기 속도
	ProjectileMovement->MaxSpeed = 1000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true; // 이동 방향을 바라보게 설정
	ProjectileMovement->ProjectileGravityScale = 1.0f; // 중력 적용 (기본 중력)
	ProjectileMovement->bShouldBounce = false; // 바운스 없음

	InitialLifeSpan = 5.0f;
}

// Called when the game starts or when spawned
void ABoss1_Projectile_Mass::BeginPlay()
{
	Super::BeginPlay();

}

void ABoss1_Projectile_Mass::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),             // 월드 포인터
			RemovalParticle,      // 파티클 시스템 에셋
			GetActorLocation(),               // 소환할 위치
			GetActorRotation(),               // 회전
			FVector(1.0f),                  // 스케일
			true                    // 자동 소멸 여부 (파티클이 끝나면 사라짐)
		);
	}
}

void ABoss1_Projectile_Mass::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this) // 자신과의 겹침을 방지
	{
		if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
		{
			Player->SetCharacterHP(-Damage);
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RemovalSound, GetActorLocation());
			Destroy();
		}
	}
}

void ABoss1_Projectile_Mass::OnCoreBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this) // 자신과의 겹침을 방지
	{
		if (OtherActor->IsA(ALandscape::StaticClass()))
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RemovalSound, GetActorLocation());
			UGameplayStatics::SpawnForceFeedbackAtLocation(GetWorld(), RemovalForceFeedbackEffect, GetActorLocation());
			Destroy();
		}
	}
}

