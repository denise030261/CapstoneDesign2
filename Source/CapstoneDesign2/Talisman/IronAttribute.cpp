// Fill out your copyright notice in the Description page of Project Settings.


#include "IronAttribute.h"

void UIronAttribute::Attack_Implementation(UWorld* World, AActor* OtherActor, ATalisman* ThisTalisman)
{
	UE_LOG(LogTemp, Log, TEXT("Iron Attack"));
}

void UIronAttribute::Passive_Implementation(UWorld* World)
{
	UE_LOG(LogTemp, Warning, TEXT("Iron Passive"));
}
