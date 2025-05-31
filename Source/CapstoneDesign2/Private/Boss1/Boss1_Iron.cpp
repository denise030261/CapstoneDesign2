// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_Iron.h"

#include "Landscape.h"
#include "CapstoneDesign2/Talisman/FireAttribute.h"
#include "CapstoneDesign2/Talisman/TalismanDataAsset.h"
#include "Components/SphereComponent.h"

// Sets default values
ABoss1_Iron::ABoss1_Iron()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	StaticMeshComponent->SetRelativeScale3D(FVector(2.0f));
	StaticMeshComponent->SetEnableGravity(true);
	StaticMeshComponent->BodyInstance.bLockXRotation = true;
	StaticMeshComponent->BodyInstance.bLockYRotation = true;
	StaticMeshComponent->BodyInstance.bLockZRotation = true;
	RootComponent = StaticMeshComponent;
	
	StaticMeshComponent->BodyInstance.bNotifyRigidBodyCollision = true;
	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ABoss1_Iron::OnHit);

	for (FSoftObjectPath Path: MeshPaths)
	{
		Meshes.Add(Cast<UStaticMesh>(Path.TryLoad()));
	}
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	SphereComponent->SetSphereRadius(50.0f);
	SphereComponent->SetupAttachment(StaticMeshComponent);
}

// Called when the game starts or when spawned
void ABoss1_Iron::BeginPlay()
{
	Super::BeginPlay();
	
	Disable();
}

void ABoss1_Iron::SetRandomMesh()
{
	const int32 Index = FMath::RandRange(0, Meshes.Num() - 1);
	StaticMeshComponent->SetStaticMesh(Meshes[Index]);
}

void ABoss1_Iron::Enable(const FVector Location, const FRotator Rotation)
{
	SetActorLocationAndRotation(Location, Rotation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	StaticMeshComponent->SetSimulatePhysics(true);
	
	Enabled = true;
}

void ABoss1_Iron::Disable()
{
	SetActorLocation(FVector(0, 0, -10000));
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	StaticMeshComponent->SetSimulatePhysics(false);
	SetRandomMesh();

	Enabled = false;
}

void ABoss1_Iron::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this) // 자신과의 겹침을 방지
	{
		if (OtherActor->IsA(ALandscape::StaticClass()))
		{
			StaticMeshComponent->SetSimulatePhysics(false);
		}
	}
}