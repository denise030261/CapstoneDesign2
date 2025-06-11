// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Talisman/Talisman.h"
#include "MainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS(config = Game)
class CAPSTONEDESIGN2_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CloseAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RangedAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BallAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GodAttackAction;
public:
	AMainCharacter();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// AttackCombo Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TArray<UAnimMontage*> DamageMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* GetupMontage;

	// Call Notify Animation
	UFUNCTION()
	void HandleOnMontageNotifyComponent(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Talisman")
	TSubclassOf<ATalisman> Talisman;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> UIPlayerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	int32 MaxHP = 100;

	// All Normal Talismans
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Talisman")
	TArray<UTalismanDataAsset*> NormalTalismanAssets;

	// All Fire Talismans
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Talisman")
	TArray<UTalismanDataAsset*> FireTalismanAssets;

	UFUNCTION(BlueprintCallable)
	int32 GetCharacterHP() { return HP; }

	UFUNCTION(BlueprintCallable)
	void SetCharacterHP(int Num);

	UFUNCTION()
	void InitCharacterHP() { HP = MaxHP; }

	UFUNCTION()
	void Dancing();

	// Attacking
	UPROPERTY(EditAnywhere)
	TArray<int> DamageArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CurAttribute = "Normal";

	// No Damage Time
	UPROPERTY(EditAnywhere)
	int NoDamageTime;

	// Force Feedback (GamePad)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GamePad")
	UForceFeedbackEffect * ForceFeedback;

	UPROPERTY(BlueprintReadOnly)
	UTalismanDataAsset* SelectedTalismanDataAsset;
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Atteck Section */
	void Attack();

	void CloseAttack(const FInputActionValue& Value);
	
	void RangedAttack(const FInputActionValue& Value);

	void MoveAttack(const FInputActionValue& Value);

	void BallAttack(const FInputActionValue& Value);

	void SpecialAttack(const FInputActionValue& Value);

	void GodAttack(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	// Attacking
	UPROPERTY(BlueprintReadOnly)
	bool bAttacking;

	// Moving
	UPROPERTY(BlueprintReadOnly)
	bool bMovement;

	// Be Damaged Level
	UPROPERTY(BlueprintReadOnly)
	int DamageLevel;

private:
	UFUNCTION()
	void ThrowTalisman();

	// Comboing
	int32 AttackComboIndex = 0;

	// Character Animation Instance
	UAnimInstance* AnimInst;

	// Montage End Delegate
	FOnMontageEnded MontageEndDelegate;

	FRotator ThrowRotation;

	UPROPERTY()
	TObjectPtr<class UUserWidget> UIPlayerInstance;

	// Play Skill Effect?
	bool bSkillEffect = false; 

	FRotator ForwardRotation;

	int32 HP;

	APlayerController* PC;

	void EnableMovement();

	bool bNoDamage = false;

	void AddNoDamage();

	FTimerHandle NoDamageTimer;

	TArray<FTimerHandle> NormalTalismanTimeHandler;

	TArray<FTimerHandle> FireTalismanTimeHandler;

	TArray<bool> bNormalTalismanUses;

	TArray<bool> bFireTalismanUses;
	
	void OnSkillCooldownFinished(int32 TalismanAttribute, int32 SkillIndex);

	void InitTalismanState();

	void PlayAttackAnimation(ATalisman* TalismanObject);

	int SelectedAttackIndex = 0;

	bool bAttackEnable();
};
