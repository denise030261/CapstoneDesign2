// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_Projectile_Needle.h"

#include "Landscape.h"
#include "CapstoneDesign2/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

class AMainCharacter;
// Sets default values
ABoss1_Projectile_Needle::ABoss1_Projectile_Needle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxCollider"));
	CollisionComponent->SetBoxExtent(FVector(83.0f, 10.0f, 5.0f));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetRelativeScale3D(FVector(1.0f));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABoss1_Projectile_Needle::OnBeginOverlap);
	RootComponent = CollisionComponent;

	CoreCollider = CreateDefaultSubobject<USphereComponent>(FName("CoreCollider"));
	CoreCollider->InitSphereRadius(5.0f);
	CoreCollider->OnComponentBeginOverlap.AddDynamic(this, &ABoss1_Projectile_Needle::OnCoreBeginOverlap);
	CoreCollider->SetupAttachment(RootComponent);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMesh"));
	MeshComponent->SetRelativeLocation(FVector(-87.0f, 0.0f, 0.0f));
	MeshComponent->SetRelativeRotation(FRotator(90.0f, 90.0f, 0.0f));
	MeshComponent->SetRelativeScale3D(FVector(2.0f, 0.4f, 2.0f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Cai_kim_-_Needle.Cai_kim_-_Needle'"));
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
	ProjectileMovement->InitialSpeed = 3000.0f; // 초기 속도
	ProjectileMovement->MaxSpeed = 3000.0f;     // 최대 속도
	ProjectileMovement->bRotationFollowsVelocity = false; // 발사체가 이동 방향에 맞춰 회전
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 중력 적용 (기본 중력)
	ProjectileMovement->bShouldBounce = false;  // 튕기도록 설정

	InitialLifeSpan = 5.0f;
}

// Called when the game starts or when spawned
void ABoss1_Projectile_Needle::BeginPlay()
{
	Super::BeginPlay();

}

void ABoss1_Projectile_Needle::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void ABoss1_Projectile_Needle::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this) // 자신과의 겹침을 방지
	{
		if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
		{
			Player->SetCharacterHP(-Damage);
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RemovalSound, GetActorLocation(), RemovalSoundVolumeMultiplier);
			Destroy();
		}
	}
}

void ABoss1_Projectile_Needle::OnCoreBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this) // 자신과의 겹침을 방지
	{
		if (OtherActor->IsA(ALandscape::StaticClass()))
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RemovalSound, GetActorLocation(), RemovalSoundVolumeMultiplier);
			UGameplayStatics::SpawnForceFeedbackAtLocation(GetWorld(), RemovalForceFeedbackEffect, GetActorLocation());
			Destroy();
		}
	}
}
