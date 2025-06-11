// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Boss1_MeleeAttackShock.h"
#include "Boss1_Projectile_Mass.h"
#include "Boss1_Projectile_Needle.h"
#include "Damageable.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Boss1.generated.h"

class UForceFeedbackComponent;
class UBoxComponent;
class UNiagaraComponent;
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
};

UCLASS()
class CAPSTONEDESIGN2_API ABoss1 : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss1();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Component")
	UBoxComponent* WeaponColliderL;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Component")
	UBoxComponent* WeaponColliderR;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Component")
	UNiagaraComponent* ShieldParticleComp;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UAudioComponent* FootstepSoundComp;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	USkeletalMesh* Phase1Mesh;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	USkeletalMesh* Phase2Mesh;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	EBoss1_State State = EBoss1_State::Spawn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	EBoss1_Pattern_State PatternState = EBoss1_Pattern_State::None;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	int32 Phase = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	bool IsActivate = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|HP", meta = (ClampMin = "0.0", Const))
	float MaxHp = 2500.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State|HP")
	float NowHp;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Property")
	float IdleSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property", meta = (ClampMin = "0.0", Const))
	float IdleSecondBase = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property", meta = (ClampMin = "0.0", Const))
	float MoveSpeedBase1 = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property", meta = (ClampMin = "0.0", Const))
	float MoveSpeedBase2 = 550.0f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Property")
	AMainCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property|Phase1", meta = (ClampMin = "0.0", Const))
	float Phase1Second = 3.0f;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Property|Phase1")
	float Phase1RemainSecond = Phase1Second;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property|Phase2", meta = (ClampMin = "0.0", Const))
	TArray<float> HealStartHps = { MaxHp * 0.75f, MaxHp * 0.5f };

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Property|Phase2")
	int32 HealStartHpsIndex = 0;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State|Phase2")
	bool IsHealPattern = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property|Phase2", meta = (ClampMin = "0.0", Const))
	float HealSecond = 30.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Property|Phase2")
	float HealRemainSecond = HealSecond;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property|Phase3", meta = (ClampMin = "0.0", Const))
	float Phase3StartHp = MaxHp * 0.25f;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pattern|Iron")
	int32 NowIronCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron", meta = (ClampMin = "0", Const))
	int32 MaxIronCount = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron", meta = (ClampMin = "0.0", Const))
	float EatIronScaleFactor = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron", meta = (ClampMin = "0.0", Const))
	float EatIronDamageFactor = 1.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron", meta = (ClampMin = "0.0", Const))
	float EatIronHealValue = MaxHp * 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|Iron", meta = (ClampMin = "0.0", Const))
	float EatIronTime = 3.0f;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pattern|Iron")
	ABoss1_IronGenerator* IronGenerator;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pattern|Iron")
	ABoss1_Iron* TargetIron;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Pattern|ShootNeedle")
	TSubclassOf<ABoss1_Projectile_Needle> NeedleProjectile = ABoss1_Projectile_Needle::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ShootNeedle", meta = (ClampMin = "0.0", Const))
	float ShootNeedleProb = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ShootNeedle", meta = (ClampMin = "0.0", Const))
	float ShootNeeleDamage = 10.0f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Pattern|ThrowMass")
	TSubclassOf<ABoss1_Projectile_Mass> MassProjectile = ABoss1_Projectile_Mass::StaticClass();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ThrowMass", meta = (ClampMin = "0.0", Const))
	float ThrowMassProb = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|ThrowMass", meta = (ClampMin = "0.0", Const))
	float ThrowMassDamage = 10.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pattern|MeleeAttack")
	bool CanDamageMeleeAttack = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack", meta = (ClampMin = "0.0", Const))
	float MeleeAttackProb = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack", meta = (ClampMin = "0.0", Const))
	float MeleeAttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern|MeleeAttack", meta = (ClampMin = "0.0", Const))
	float MeleeAttackShockDamage = 10.0f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Pattern|MeleeAttack")
	TSubclassOf<ABoss1_MeleeAttackShock> MeleeAttackShock = ABoss1_MeleeAttackShock::StaticClass();
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* SpawnMontage;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* PatternMontage;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TArray<UAnimMontage*> MeleeAttackMontages;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DieMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* SpawnSound;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* EatSound;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* PatternStartSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* ProjectileStartSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* MeleeAttackStartSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* MeleeAttackHitSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundCue* HealStartSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Text")
	UStringTable* QuestTextStringTable;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "GamePad")
	UForceFeedbackEffect* GrowlForceFeedbackEffect;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "GamePad")
	UForceFeedbackEffect* SoundForceFeedbackEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GamePad")
	UForceFeedbackEffect* HitForceFeedbackEffect;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapBegin_Weapon(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void DealDamage(float DamageAmount, const UTalismanDataAsset* DataAsset) override;
	
private:
	void CheckState(float DeltaTime);

	void SetStateIdle();
	void SetTargetIron();
	void EatIron(ABoss1_Iron* Iron);
	void EndEatIron();
	void Trace(float DeltaTime);
	void Aiming(float DeltaTime);

	void MoveToIron(float DeltaTime);
	
	void ShootNeedleStart();
	UFUNCTION()
	void ShootNeedle();
	UFUNCTION()
	void ShootNeedleEnd();
	
	void ThrowMassStart();
	UFUNCTION()
	void ThrowMass();
	UFUNCTION()
	void ThrowMassEnd();

	void EndPattern();

	void UpdateWeaponCollider() const;
	
	void MeleeAttackStart();
	UFUNCTION()
	void MeleeAttack();
	UFUNCTION()
	void MeleeAttackEnd();
	UFUNCTION()
	void MeleeAttackDelayEnd();

	void HealStart();

	void SetPhase1();
	void SetPhase2();
	void SetPhase3();
	void SetDie();
	
	void InitGrow(const float StartScale, const float EndScale, const float TotalTime);
	void Grow(float DeltaTime);
	float GrowStartScale;
	float GrowEndScale;
	float GrowDeltaTime = 0.0f;
	float GrowTotalTime;

	FRotator CalcSmoothLookAtRotation(const FVector& Location, float DeltaTime) const;

	void SetQuestStringFromStringTable(const FString& Key) const;
	
	FTimerHandle Phase1TimerHandle;
	FTimerHandle IdleTimerHandle;
	FTimerHandle CastingTimerHandle;
	TArray<FTimerHandle> ShockHandles;
	FTimerHandle HealTimerHandle;
};
