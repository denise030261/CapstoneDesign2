// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1UI.h"

#include "Boss1/Boss1.h"

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

		if (Boss1->Phase == 1 && Boss1->IsActivate)
		{
			const int32 Time = FMath::FloorToInt(Boss1->Phase1RemainSecond);
			const int32 Minute = Time / 60;
			const int32 Second = Time % 60;
			
			TimerString = FString::Printf(TEXT("%02d:%02d"), Minute, Second);
		}
		else if (Boss1->IsHealPattern)
		{
			const int32 Time = FMath::FloorToInt(Boss1->HealRemainSecond);
			const int32 Minute = Time / 60;
			const int32 Second = Time % 60;
			
			TimerString = FString::Printf(TEXT("%02d:%02d"), Minute, Second);
		}
		else
		{
			TimerString = TEXT("");
		}
	}
}

