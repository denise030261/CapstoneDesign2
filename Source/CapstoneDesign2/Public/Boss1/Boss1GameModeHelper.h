// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boss1GameModeHelper.generated.h"

class ABoss1;
/**
 * 
 */
UCLASS(Blueprintable)
class CAPSTONEDESIGN2_API UBoss1GameModeHelper : public UObject
{
	GENERATED_BODY()
	
public:
	UBoss1GameModeHelper();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SpawnPointActor;

	UFUNCTION(BlueprintCallable)
	FVector FindBossSpawnLocation() const;

	UFUNCTION(Blueprintable)
	FRotator FindBossSpawnRotation() const;

	UFUNCTION(BlueprintCallable)
	FRotator FindPlayerSpawnRotation() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	ABoss1* Boss1;

	UFUNCTION(BlueprintCallable, Category = "Boss")
	ABoss1* GenerateBoss1(FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(BlueprintCallable)
	void SetStage();
	
};
