// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boss1/Boss1_Iron.h"
#include "GameFramework/Actor.h"
#include "Boss1_IronGenerator.generated.h"

class ABoss1_Iron;

UCLASS()
class CAPSTONEDESIGN2_API ABoss1_IronGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoss1_IronGenerator();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	USceneComponent* SceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float GenerateCoolTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float SpawnRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	int32 MaxIronCount = 20;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Iron")
	TSubclassOf<ABoss1_Iron> IronToSpawn = ABoss1_Iron::StaticClass();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Iron")
	TSet<ABoss1_Iron*> SpawnedIrons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Iron")
	TArray<ABoss1_Iron*> IronPool;

	UFUNCTION(BlueprintCallable)
	void RemoveIron(ABoss1_Iron* Iron);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	FTimerHandle GenerateTimerHandle;

	void InitializeIronPool();
	void GenerateIron();
	ABoss1_Iron* GetIronFromPool();
};
