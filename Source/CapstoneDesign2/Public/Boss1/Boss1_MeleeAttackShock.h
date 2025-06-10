// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Boss1_MeleeAttackShock.generated.h"

UCLASS()
class CAPSTONEDESIGN2_API ABoss1_MeleeAttackShock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoss1_MeleeAttackShock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool CanDealDamage = true;
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UParticleSystemComponent* ParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USoundCue* ShockSound;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GamePad")
	UForceFeedbackEffect* ShockForceFeedbackEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	float PersistentTime = 1.5f;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnParticleSystemFinished(UParticleSystemComponent* PSystem);
};

