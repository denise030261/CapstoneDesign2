// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boss1/Boss1_Base.h"
#include "Boss1/Boss1_Phase2.h"
#include "Boss1_Phase1.generated.h"

class AMainCharacter;
class ABoss1_Projectile_Mass;
class ABoss1_Projectile_Needle;
class ABoss1_Iron;
class ABoss1_IronGenerator;
class ABoss1_Phase2;

UCLASS()
class CAPSTONEDESIGN2_API ABoss1_Phase1 : public ABoss1_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss1_Phase1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float Phase1Second = 30.0f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Property")
	TSubclassOf<ABoss1_Phase2> Boss1_Phase2ToSpawn = ABoss1_Phase2::StaticClass();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	int32 MaxIronCount = 3;
	
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	virtual void DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset) override;
	
private:
	virtual void CheckState(float DeltaTime) override;

	virtual void Trace(float DeltaTime) override;

	virtual void ShootNeedle() override;
	
	void SetToPhase2();

	FTimerHandle Phase1TimerHandle;
};
