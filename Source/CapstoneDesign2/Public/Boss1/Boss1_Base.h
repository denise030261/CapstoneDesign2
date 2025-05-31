// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damageable.h"
#include "Boss1/Boss1_Projectile_Mass.h"
#include "Boss1/Boss1_Projectile_Needle.h"
#include "GameFramework/Character.h"
#include "Boss1_Base.generated.h"

class ABoss1_Projectile_Mass;
class ABoss1_Iron;
class ABoss1_IronGenerator;
class AMainCharacter;

UENUM(BlueprintType)
enum class EBoss1_State : uint8
{
	Spawn UMETA(DisplayName = "Spawn"),
	Idle UMETA(DisplayName = "Idle"),
	Tracing UMETA(DisplayName = "Tracing"),
	Eating UMETA(DisplayName = "Eating"),
	Aiming UMETA(DisplayName = "Aiming"),
	Casting UMETA(DisplayName = "Casting"),
	Die UMETA(DisplayName = "Die"),
};

UENUM()
enum class EBoss1_Pattern_State : uint8
{
	None UMETA(DisplayName = "None"),
	ShootNeedle UMETA(DisplayName = "ShootNeedle"),
	ThrowMass UMETA(DisplayName = "ThrowMass"),
	MeleeAttack UMETA(DisplayName = "MelleAttack"),
	Heal UMETA(DisplayName = "Heal"),
};

UCLASS(Abstract)
class CAPSTONEDESIGN2_API ABoss1_Base : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss1_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EBoss1_State State = EBoss1_State::Spawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EBoss1_Pattern_State PatternState = EBoss1_Pattern_State::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HP")
	float MaxHp = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	float NowHp = 5000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float IdleSecond = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float IdleSecondBase = 1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float MoveSpeed = 150.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	AMainCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	int32 NowIronCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	int32 MaxIronCount = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	float EatIronScaleFactor = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	float EatIronDamageFactor = 1.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	float EatIronHealValue = MaxHp * 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	float EatIronTime = 3.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	ABoss1_IronGenerator* IronGenerator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	ABoss1_Iron* TargetIron;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Pattern|ShootNeedle")
	TSubclassOf<ABoss1_Projectile_Needle> NeedleProjectile = ABoss1_Projectile_Needle::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ShootNeedle")
	float ShootNeedleProb = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ShootNeedle")
	float ShootNeeleDamage = 30.0f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Pattern|ThrowMass")
	TSubclassOf<ABoss1_Projectile_Mass> MassProjectile = ABoss1_Projectile_Mass::StaticClass();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ThrowMass")
	float ThrowMassProb = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ThrowMass")
	float ThrowMassDamage = 30.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* PatternMontage;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) PURE_VIRTUAL(ABoss1_Base::OnHit, );
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) PURE_VIRTUAL(ABoss1_Base::OnOverlapBegin, );

protected:
	virtual void CheckState(float DeltaTime) PURE_VIRTUAL(ABoss1_Base::CheckState, );

	void SetStateIdle();
	void SetTargetIron();
	void EatIron(ABoss1_Iron* Iron);
	virtual void EndEatIron() PURE_VIRTUAL(ABoss1_Base::EndEatIron, );
	virtual void Trace(float DeltaTime) PURE_VIRTUAL(ABoss1_Base::Trace, );
	void Aiming(float DeltaTime);

	void MoveToIron(float DeltaTime);
	
	void ShootNeedleStart();
	UFUNCTION()
	virtual void ShootNeedle() PURE_VIRTUAL(ABoss1_Base::ShootNeedle, );
	UFUNCTION()
	void ShootNeedleEnd();
	
	void ThrowMassStart();
	UFUNCTION()
	void ThrowMass();
	UFUNCTION()
	void ThrowMassEnd();

	void EndPattern();

	void InitGrow(const float StartScale, const float EndScale, const float TotalTime);
	void Grow(float DeltaTime);
	float GrowStartScale;
	float GrowEndScale;
	float GrowDeltaTime = 0.0f;
	float GrowTotalTime;

	FRotator CalcSmoothLookAtRotation(const FVector& Location, float DeltaTime) const;
	
	FTimerHandle IdleTimerHandle;
	FTimerHandle CastingTimerHandle;
};
