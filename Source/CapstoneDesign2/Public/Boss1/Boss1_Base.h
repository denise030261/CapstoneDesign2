// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boss1/Boss1_Projectile_Mass.h"
#include "Boss1/Boss1_Projectile_Needle.h"
#include "GameFramework/Character.h"
#include "Boss1_Base.generated.h"

class ABoss1_Projectile_Mass;
class ABoss1_Iron;
class ABoss1_IronGenerator;
class AMainCharacter;

UENUM()
enum class EBoss1_State : uint8
{
	Spawn UMETA(DisplayName = "Spawn"),
	Idle UMETA(DisplayName = "Idle"),
	Tracing UMETA(DisplayName = "Tracing"),
	Aiming UMETA(DisplayName = "Aiming"),
	Casting UMETA(DisplayName = "Casting"),

};

UENUM()
enum class EBoss1_Pattern_State : uint8
{
	None UMETA(DisplayName = "None"),
	ShootNeedle UMETA(DisplayName = "ShootNeedle"),
	ThrowMass UMETA(DisplayName = "ThrowMass"),
	MeleeAttack UMETA(DisplayName = "MelleAttack")
};

UCLASS(Abstract)
class CAPSTONEDESIGN2_API ABoss1_Base : public ACharacter
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	int32 IdleSecond = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float MoveSpeed = 150.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	AMainCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	int32 NowIronCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	ABoss1_IronGenerator* IronGenerator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pattern|Iron")
	ABoss1_Iron* TargetIron;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Pattern|ShootNeedle")
	TSubclassOf<ABoss1_Projectile_Needle> NeedleProjectile = ABoss1_Projectile_Needle::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ShootNeedle")
	float ShootNeedleProb = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ShootNeedle")
	float ShootNeeleDamage;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Pattern|ThrowMass")
	TSubclassOf<ABoss1_Projectile_Mass> MassProjectile = ABoss1_Projectile_Mass::StaticClass();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ThrowMass")
	float ThrowMassProb = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ThrowMass")
	float ThrowMassDamage = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* PatternMontage;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) PURE_VIRTUAL(ABoss1_Base::OnHit, );
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) PURE_VIRTUAL(ABoss1_Base::OnOverlapBegin, );

protected:
	virtual void CheckState(float DeltaTime);

	void SetStateIdle();
	void SetTargetIron();
	void EatIron(ABoss1_Iron* Iron);
	virtual void Trace(float DeltaTime);
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

	FRotator CalcSmoothLookAtRotation(const FVector& Location, float DeltaTime) const;
	
	FTimerHandle IdleTimerHandle;
	FTimerHandle CastingTimerHandle;
};
