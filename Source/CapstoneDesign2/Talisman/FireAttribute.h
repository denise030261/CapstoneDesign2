// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalismanAttributeStrategy.h"
#include "SpawnSkill.h"
#include "FireAttribute.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UFireAttribute : public UTalismanAttributeStrategy
{
	GENERATED_BODY()
	
public:
	UFireAttribute();

	virtual void Attack_Implementation(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman) override;
	virtual void Passive_Implementation(UWorld* World) override;

	UPROPERTY(EditDefaultsOnly, Category = "Talisman")
	TSubclassOf<ASpawnSkill> FireActor;
};
