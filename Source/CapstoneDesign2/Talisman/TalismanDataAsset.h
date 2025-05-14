// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Particles/ParticleSystem.h"
#include "TalismanAttributeStrategy.h"
#include "TalismanSkillStrategy.h"
#include "NiagaraSystem.h"
#include "TalismanDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSkillData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Distance;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UNiagaraSystem* Effect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UNiagaraSystem* ExecuteEffect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UNiagaraSystem* SkillEffect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AActor> SpawnSkill;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UTalismanAttributeStrategy> Attribute;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UTalismanSkillStrategy> Skill;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString AnimationType;
};

UCLASS()
class CAPSTONEDESIGN2_API UTalismanDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FSkillData SkillInfo;
	
};
