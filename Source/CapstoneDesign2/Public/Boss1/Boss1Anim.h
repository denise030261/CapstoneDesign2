// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Boss1Anim.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShootNeedleAimingEndNotify);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnThrowMassAimingEndNotify);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndOnceNotify);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndAllNotify);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeleeAttackStartNotify);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeleeAttackEndNotify);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeleeAttackDelayEndNotify);

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UBoss1Anim : public UAnimInstance
{
	GENERATED_BODY()

private:
	const float MinAnimMoveSpeed = 150.0f;
	const float MaxAnimMoveSpeed = 550.0f;
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly)
	float AnimMoveSpeed;

	UPROPERTY(BlueprintReadOnly)
	float AnimMoveDirection;

	UPROPERTY(BlueprintReadOnly)
	float MoveAnimationPlayRate = 1.0f;
	
	UFUNCTION(BlueprintCallable)
	bool CanTransitionToMove() const;

	UFUNCTION()
	void AnimNotify_ShootNeedleAimingEndNotify();

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnShootNeedleAimingEndNotify OnShootNeedleAimingEndNotify;

	UFUNCTION()
	void AnimNotify_ThrowMassAimingEndNotify();

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnThrowMassAimingEndNotify OnThrowMassAimingEndNotify;
	
	UFUNCTION()
	void AnimNotify_EndOnceNotify();

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnEndOnceNotify OnEndOnceNotify;
	
	UFUNCTION()
	void AnimNotify_EndAllNotify();

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnEndAllNotify OnEndAllNotify;

	UFUNCTION()
	void AnimNotify_MeleeAttackStartNotify();

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnMeleeAttackStartNotify OnMeleeAttackStartNotify;
	
	UFUNCTION()
	void AnimNotify_MeleeAttackEndNotify();

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnMeleeAttackEndNotify OnMeleeAttackEndNotify;

	UFUNCTION()
	void AnimNotify_MeleeAttackDelayEndNotify();

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnMeleeAttackDelayEndNotify OnMeleeAttackDelayEndNotify;
	
private:
	float CalcMoveAnimationPlayRate(const ACharacter* Character) const;
	float CalcSpeed(const ACharacter* Character) const;
	static float CalcDirectionAngle(const ACharacter* Character);

};
