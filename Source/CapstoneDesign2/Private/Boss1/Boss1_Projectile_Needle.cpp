// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_Projectile_Needle.h"

#include "CapstoneDesign2/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceConstant.h"

class AMainCharacter;
// Sets default values
ABoss1_Projectile_Needle::ABoss1_Projectile_Needle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxCollider"));
	CollisionComponent->SetBoxExtent(FVector(50.0f));
	CollisionComponent->SetRelativeScale3D(FVector(1.0f, 0.1f, 0.1f) * 1.5f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABoss1_Projectile_Needle::OnBeginOverlap);
	RootComponent = CollisionComponent;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMesh"));
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (MeshAsset.Succeeded()) MeshComponent->SetStaticMesh(MeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MaterialAsset(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Fab/Megascans/Surfaces/Rusty_Metal_Sheet_tj4kedvcw/Raw/MI_tj4kedvcw.MI_tj4kedvcw'"));
	if (MaterialAsset.Succeeded()) MeshComponent->SetMaterial(0, MaterialAsset.Object);

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

void ABoss1_Projectile_Needle::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this) // 자신과의 겹침을 방지
	{
		if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
		{
			// Damage to Player
			Destroy();
		}
	}
}


