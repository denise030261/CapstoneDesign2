// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "TalismanDataAsset.h"
#include "Talisman.generated.h"

UCLASS()
class CAPSTONEDESIGN2_API ATalisman : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATalisman();

	// When Other Detect, Effect and Destroy
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Talisman Move Distance
	UFUNCTION()
	void SetMoveDistance(FVector3d StartLocation);

	// Move Talisman
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	UTalismanDataAsset* TalismanDataAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* TalismanMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

	bool bRangeAttack = false; // if RangeAttack Strategy and Overlap, Call RangeAttack Function

	UPROPERTY()
	bool bIsDestroyed = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FVector3d TargetDistance;

};
