// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/Monster1/Monster1Anim.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Monsters/Monster1/Monster1.h"

void UMonster1Anim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	
}

void UMonster1Anim::AnimNotify_AttackEndNotify() const
{
	OnAttackEndNotify.Broadcast();
}