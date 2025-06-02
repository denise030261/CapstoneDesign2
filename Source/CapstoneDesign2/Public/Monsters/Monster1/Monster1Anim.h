// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Monsters/Monster0/Monster0Anim.h"
#include "Monster1Anim.generated.h"

enum class EMonster1_State : uint8;
/**
 * 
 */
UCLASS()
class CAPSTONEDESIGN2_API UMonster1Anim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float AnimMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	EMonster1_State AnimState;
	
	UFUNCTION()
	void AnimNotify_AttackEndNotify() const;

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnAttackEndNotify OnAttackEndNotify;

	UFUNCTION()
	void AnimNotify_DieEndNotify() const;

	UPROPERTY(BlueprintAssignable, Category = "Animation Notifies")
	FOnDieEndNotify OnDieEndNotify;
};
