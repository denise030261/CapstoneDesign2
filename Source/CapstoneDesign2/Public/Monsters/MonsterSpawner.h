// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

class AMonsterBase;

UCLASS()
class CAPSTONEDESIGN2_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonsterSpawner();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	USceneComponent* SceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float ReGenerateCoolTime = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float SpawnRadius = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	int32 MaxMonsterCount = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	TSubclassOf<AMonsterBase> MonsterToSpawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TArray<AMonsterBase*> Monsters;

	void GenerateMonster();
	void CheckMonsterCount();
};
