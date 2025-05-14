// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalismanSkillStrategy.h"
#include "NiagaraSystem.h"
#include <Components/SphereComponent.h>
#include "SpawnSkill.generated.h"

UCLASS()
class CAPSTONEDESIGN2_API ASpawnSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnSkill();

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Effect")
	UNiagaraSystem* NiagaraSystem; 

	UPROPERTY(EditAnywhere, Category = "Destroy")
	float DestroyTime = 0;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* TriggerVolume;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 SpawnCount = 0;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnDist = 0;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float Speed = 10;

	UFUNCTION()
	void SpawnMove(FVector3d StartLocation);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle DestroyHandle; // Destroy Timer
	ATalisman* OwnTalisman;
	AActor* TargetActor;
	FVector3d TargetDistance;
};
