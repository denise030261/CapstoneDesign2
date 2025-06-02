// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damageable.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"

class AMainCharacter;
class UWidgetComponent;

UCLASS(Abstract)
class CAPSTONEDESIGN2_API AMonsterBase : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonsterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> MonsterUI;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	AMainCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float NowHp;

protected:
	virtual void UpdateUI() const;
	virtual void UpdateAnimInstance() const;
	virtual void SetDie();
	
	float CalcDistance() const;
	
	FRotator CalcSmoothLookAtRotation(const FVector& Location, const float DeltaTime) const;
	void Gaze(const float DeltaTime);
};
