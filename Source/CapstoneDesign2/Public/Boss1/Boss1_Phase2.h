// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boss1_MeleeAttackShock.h"
#include "Boss1_RageShock.h"
#include "Boss1/Boss1_Base.h"
#include "Boss1_Phase2.generated.h"

class UNiagaraComponent;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool IsRage = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float Phase2MoveSpeed = 550.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float RageStartHp = MaxHp * 0.25f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	UBoxComponent* WeaponColliderL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	UBoxComponent* WeaponColliderR;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	float MeleeAttackProb = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	float MeleeAttackDamage = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	float MeleeAttackShockDamage = 30.0f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Pattern|MeleeAttack")
	TSubclassOf<ABoss1_MeleeAttackShock> MeleeAttackShock = ABoss1_MeleeAttackShock::StaticClass();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TArray<UAnimMontage*> MeleeAttackMontages;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Pattern|Rage")
	TSubclassOf<ABoss1_RageShock> RageShock = ABoss1_RageShock::StaticClass();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DieMontage;
	
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnOverlapBegin_Weapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset) override;
	
private:
	bool CanDamageMeleeAttack = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	virtual void CheckState(float DeltaTime) override;

	virtual void Trace(float DeltaTime) override;

	virtual void EndEatIron() override;
	
	void MeleeAttackStart();
	UFUNCTION()
	void MeleeAttack();
	UFUNCTION()
	void MeleeAttackEnd();
	UFUNCTION()
	void MeleeAttackDelayEnd();
	
	virtual void ShootNeedle() override;
	
	void SetRage();
	void SetDie();
};
