// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boss1_Iron.generated.h"

class USphereComponent;

UCLASS()
class CAPSTONEDESIGN2_API ABoss1_Iron : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoss1_Iron();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TArray<FSoftObjectPath> MeshPaths =
	{
		//FSoftObjectPath(TEXT("/Script/Engine.StaticMesh'/Game/Fab/Megascans/3D/Industrial_Junkyard_Crate_Metal_wdlifhrs/Raw/wdlifhrs_tier_0.wdlifhrs_tier_0'")),
		FSoftObjectPath(TEXT("/Script/Engine.StaticMesh'/Game/Fab/Megascans/3D/Metal_Pot_vfyqcj2ga/Raw/SM_vfyqcj2ga_0.SM_vfyqcj2ga_0'")),
		FSoftObjectPath(TEXT("/Script/Engine.StaticMesh'/Game/Fab/Megascans/3D/Old_Metal_Jug_ubitbbifa/Raw/ubitbbifa_tier_0.ubitbbifa_tier_0'")),
		FSoftObjectPath(TEXT("/Script/Engine.StaticMesh'/Game/Fab/Megascans/3D/Old_Metal_Pot_wdktcgq/Raw/wdktcgq_tier_0.wdktcgq_tier_0'")),
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	USphereComponent* SphereComponent;
	
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void SetRandomMesh();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pooling")
	bool Enabled = false;
	
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	void Enable(const FVector Location = FVector::ZeroVector, const FRotator Rotation = FRotator::ZeroRotator);

	UFUNCTION(BlueprintCallable, Category = "Pooling")
	void Disable();

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	TArray<UStaticMesh*> Meshes;
};
