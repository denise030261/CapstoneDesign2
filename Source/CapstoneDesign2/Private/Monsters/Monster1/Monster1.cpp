// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/Monster1/Monster1.h"

// Sets default values
AMonster1::AMonster1()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMonster1::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultController();
}

// Called every frame
void AMonster1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMonster1::DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset)
{
	
}
