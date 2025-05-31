// Fill out your copyright notice in the Description page of Project Settings.


#include "Monsters/MonsterUI.h"

void UMonsterUI::SetMonsterName(const FString Name)
{
	MonsterName = Name;
}

void UMonsterUI::SetMaxHp(const float Hp)
{
	MaxHp = Hp;
}

void UMonsterUI::SetHp(const float Hp)
{
	NowHp = Hp;
}

float UMonsterUI::GetHpPercentage() const
{
	return FMath::Clamp(NowHp / MaxHp, 0.0f, 1.0f);
}
