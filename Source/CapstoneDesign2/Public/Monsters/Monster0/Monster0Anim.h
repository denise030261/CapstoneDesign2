// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Monster0Anim.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCanDamageAttackEndNotify);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndNotify);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDieEndNotify);

/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UMonster0Anim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void AnimNotify_CanDamageAttackEndNotify() const;

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnCanDamageAttackEndNotify OnCanDamageAttackEndNotify;

	UFUNCTION()
	void AnimNotify_AttackEndNotify() const;
	
	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnAttackEndNotify OnAttackEndNotify;

	UFUNCTION()
	void AnimNotify_DieEndNotify() const;
	
	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnDieEndNotify OnDieEndNotify;
};
