// Fill out your copyright notice in the Description page of Project Settings.


#include "PassiveSkill.h"
#include "TalismanAttributeStrategy.h"
#include "Talisman.h"

void UPassiveSkill::SkillExecute_Implementation(ATalisman* Owner, UWorld* World)
{
	if (UTalismanAttributeStrategy* AttributeCDO = Owner->TalismanDataAsset->SkillInfo.Attribute->GetDefaultObject<UTalismanAttributeStrategy>())
	{
		AttributeCDO->Passive_Implementation(World);
	}
}
