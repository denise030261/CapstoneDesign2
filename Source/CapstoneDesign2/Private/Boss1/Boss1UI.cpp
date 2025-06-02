// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1UI.h"

#include "Boss1/Boss1_Base.h"

void UBoss1UI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (Boss1)
	{
		HpPercent = Boss1->NowHp / Boss1->MaxHp;
		if (Boss1->NowIronCount > 0)
		{
			IsEatIron[Boss1->NowIronCount - 1] = true;
		}
	}
}

