// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalismanAttributeStrategy.h"
#include "GameFramework/Character.h"
#include "NormalAttribute.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UNormalAttribute : public UTalismanAttributeStrategy
{
	GENERATED_BODY()
	
public:
	virtual void Attack_Implementation(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman) override;
	virtual void Passive_Implementation(UWorld* World) override;

private:
	ACharacter* Character;
};
