// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_Iron.h"

#include "Components/SphereComponent.h"

// Sets default values
ABoss1_Iron::ABoss1_Iron()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMeshComponent"));
	StaticMeshComponent->SetEnableGravity(true);
	StaticMeshComponent->BodyInstance.bLockXTranslation = true;
	StaticMeshComponent->BodyInstance.bLockYTranslation = true;
	StaticMeshComponent->BodyInstance.bLockXRotation = true;
	StaticMeshComponent->BodyInstance.bLockYRotation = true;
	StaticMeshComponent->BodyInstance.bLockZRotation = true;
	RootComponent = StaticMeshComponent;

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
	if (!Enabled)
	{
		SetActorLocation(Location);
		SetActorRotation(Rotation);
	
		SetRandomMesh();
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		StaticMeshComponent->SetSimulatePhysics(true);

		Enabled = true;
	}
}

void ABoss1_Iron::Disable()
{
	if (Enabled)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		StaticMeshComponent->SetSimulatePhysics(false);
	
		Enabled = false;
	}
}

