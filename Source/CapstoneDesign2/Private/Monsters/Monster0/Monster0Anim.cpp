// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/Monster0/Monster0Anim.h"

void UMonster0Anim::AnimNotify_CanDamageAttackEndNotify() const
{
	OnCanDamageAttackEndNotify.Broadcast();
}

void UMonster0Anim::AnimNotify_AttackEndNotify() const
{
	OnAttackEndNotify.Broadcast();
}


