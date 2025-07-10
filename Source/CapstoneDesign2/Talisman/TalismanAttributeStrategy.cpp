// Fill out your copyright notice in the Description page of Project Settings.


#include "TalismanAttributeStrategy.h"
#include <Kismet/GameplayStatics.h>

void UTalismanAttributeStrategy::PlayHitSound(UWorld* World, FVector Location)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(World, HitSound, Location);
	}
}
