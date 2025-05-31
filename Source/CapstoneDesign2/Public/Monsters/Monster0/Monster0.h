// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damageable.h"
#include "GameFramework/Character.h"
#include "Monster0.generated.h"

class AMainCharacter;
class UBoxComponent;

UENUM(BlueprintType)
enum class EMonster0_State : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Attack UMETA(DisplayName = "Attack"),
	Die UMETA(DisplayName = "Die"),
};

UCLASS()
class CAPSTONEDESIGN2_API AMonster0 : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonster0();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset) override;

	UFUNCTION()
	virtual void OnWeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* WeaponColliderL;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* WeaponColliderR;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EMonster0_State State = EMonster0_State::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	AMainCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float MaxHp = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float NowHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float AttackDamage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float AttackReadyTime = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float GazeDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float AttackDistance = 200.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DieMontage;

private:
	bool CanDamageAttack = false;
	FTimerHandle AttackReadyHandle;

	void CheckState(const float DeltaTime);
	void IdleTransition(const float DeltaTime);
	void StartAttack();
	
	UFUNCTION()
	void EndCanDamageAttack();
	UFUNCTION()
	void EndAttack();
	
	FRotator CalcSmoothLookAtRotation(const FVector& Location, const float DeltaTime) const;
	void Gaze(const float DeltaTime);
	void UpdateWeaponColliders() const;
	void SetDie();
};
