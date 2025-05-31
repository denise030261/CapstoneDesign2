// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damageable.h"
#include "GameFramework/Character.h"
#include "Monster1.generated.h"

class UMonster1Anim;
class USphereComponent;
class AMainCharacter;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EMonster1_State : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Gaze UMETA(DisplayName = "Gaze"),
	Trace UMETA(DisplayName = "Trace"),
	Attack UMETA(DisplayName = "Attack"),
	Die UMETA(DisplayName = "Die"),
};

UCLASS()
class CAPSTONEDESIGN2_API AMonster1 : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonster1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset) override;
	
	UFUNCTION()
	virtual void OnWeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> MonsterUI;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* WeaponColliderR;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EMonster1_State State = EMonster1_State::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	AMainCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float MaxHp = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float NowHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float GazeTime = 1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float TraceDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float AttackDistance = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float AttackDamage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,	Category = "Property")
	float AttackReadyTime = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DieMontage;

private:
	UPROPERTY()
	UMonster1Anim* AnimInstance;
	
	bool CanDamageAttack = false;
	FTimerHandle AttackReadyHandle;
	FTimerHandle GazeHandle;

	void CheckState(const float DeltaTime);
	void IdleTransition();
	void StartGaze();
	void GazeTransition(const float DeltaTime);
	void TraceTransition(const float DeltaTime);
	void StartAttack();
	UFUNCTION()
	void EndAttack();

	float CalcDistance() const;
	
	FRotator CalcSmoothLookAtRotation(const FVector& Location, const float DeltaTime) const;
	void Gaze(const float DeltaTime);

	void UpdateWeaponColliders() const;
	void UpdateAnimInstance() const;
	void UpdateUI() const;
	void SetDie();
};
