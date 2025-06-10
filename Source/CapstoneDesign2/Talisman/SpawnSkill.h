// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalismanSkillStrategy.h"
#include "NiagaraSystem.h"
#include "TalismanDataAsset.h"
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

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* TriggerVolume;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 SpawnCount = 0;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnDist = 0;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float Speed = 10;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float StayTime = 5;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	UTalismanDataAsset* TalismanDataAsset;

	UFUNCTION()
	void SpawnMove(FVector3d StartLocation);

	UFUNCTION()
	void SetTalisman(ATalisman* Talisman);

	UPROPERTY()
	TSet<AActor*> OverlappingDamageableActors;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle DestroyHandle; // Destroy Timer
	FTimerHandle AttackHandle; // Attack Timer
	ATalisman* OwnTalisman;
	AActor* TargetActor;
	FVector3d TargetDistance;

	UFUNCTION()
	void RepeatAttack();
};
