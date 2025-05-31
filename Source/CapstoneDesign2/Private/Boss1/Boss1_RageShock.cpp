// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1_RageShock.h"

#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABoss1_RageShock::ABoss1_RageShock()
{
	CollisionComponent->SetRelativeScale3D(FVector(3.0f));
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonGrux/FX/Particles/Abilities/HardKnocks/FX/P_Grux_Knockup.P_Grux_Knockup'"));
	ParticleComponent->SetTemplate(ParticleAsset.Object);
}