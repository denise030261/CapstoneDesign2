// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_MeleeAttackShock.h"

#include "CapstoneDesign2/MainCharacter.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"

class AMainCharacter;
// Sets default values
ABoss1_MeleeAttackShock::ABoss1_MeleeAttackShock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 충돌 컴포넌트 설정
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(FName("SphereCollider"));
	CollisionComponent->InitSphereRadius(250.0f);
	CollisionComponent->SetRelativeScale3D(FVector(0.4f));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABoss1_MeleeAttackShock::OnBeginOverlap);
	RootComponent = CollisionComponent;

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(FName("ParticleComponent"));
	ParticleComponent->SetupAttachment(RootComponent);
	ParticleComponent->OnSystemFinished.AddDynamic(this, &ABoss1_MeleeAttackShock::OnParticleSystemFinished);
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonGrux/FX/Particles/Skins/Grux_Beetle_Magma/P_MagmaHardknocks_AOE.P_MagmaHardknocks_AOE'"));
	ParticleComponent->SetTemplate(ParticleAsset.Object);
}

// Called when the game starts or when spawned
void ABoss1_MeleeAttackShock::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle CanDealDamageHandle;
	GetWorldTimerManager().SetTimer(CanDealDamageHandle, [&] { CanDealDamage = false; }, PersistentTime, false);
}

// Called every frame
void ABoss1_MeleeAttackShock::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanDealDamage && OtherActor && OtherActor != this) // 자신과의 겹침을 방지
	{
		if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
		{
			Player->SetCharacterHP(-Damage);
			CanDealDamage = false;
		}
	}
}

void ABoss1_MeleeAttackShock::OnParticleSystemFinished(UParticleSystemComponent* PSystem)
{
	Destroy();
}

