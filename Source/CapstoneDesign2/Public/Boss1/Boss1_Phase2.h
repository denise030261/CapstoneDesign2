// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boss1/Boss1_Base.h"
#include "Boss1_Phase2.generated.h"

class AMainCharacter;
class ABoss1_Projectile_Mass;
class ABoss1_Projectile_Needle;
class ABoss1_Iron;
class ABoss1_IronGenerator;

UCLASS()
class CAPSTONEDESIGN2_API ABoss1_Phase2 : public ABoss1_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss1_Phase2();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ThrowMass")
	int32 ThrowMassCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ThrowMass")
	float ThrowMassDelay = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	UBoxComponent* WeaponColliderL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	UBoxComponent* WeaponColliderR;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	float MeleeAttackProb = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	float MeleeAttackDamage = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	float MeleeAttackStartDelay = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	float MeleeAttackEndDelay = 1.0f;
	
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnOverlapBegin_Weapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	bool CanDamageMeleeAttack = false;
	int32 ThrowMassCounter;
	FTimerHandle PatternTimerHandle;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	virtual void CheckState(float DeltaTime) override;

	virtual void Trace(float DeltaTime) override;

	void MeleeAttackStart();
	void MeleeAttack();
	void MeleeAttackEnd();
	
	virtual void ShootNeedle() override;
	
	virtual void ThrowMass() override;
	
	void SetToRage();
};
