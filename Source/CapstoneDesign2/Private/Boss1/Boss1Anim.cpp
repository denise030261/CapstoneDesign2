// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss1/Boss1Anim.h"

#include "Boss1/Boss1_Base.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBoss1Anim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const ABoss1_Base* Boss1 = Cast<ABoss1_Base>(TryGetPawnOwner()))
	{
		MoveAnimationPlayRate = CalcMoveAnimationPlayRate(Boss1);
		AnimMoveSpeed = CalcSpeed(Boss1);
		AnimMoveDirection = CalcDirectionAngle(Boss1);
	}
}

float UBoss1Anim::CalcMoveAnimationPlayRate(const ACharacter* Character) const
{
	return Character->GetCharacterMovement()->GetMaxSpeed() / MinAnimMoveSpeed;
}

bool UBoss1Anim::CanTransitionToMove() const
{
	return AnimMoveSpeed >= MinAnimMoveSpeed;
}

float UBoss1Anim::CalcSpeed(const ACharacter* Character) const
{
	const float OriginSpeed = Character->GetVelocity().Size();
	const float MaxSpeed = Character->GetCharacterMovement()->GetMaxSpeed();
	const float NormalizedSpeed = OriginSpeed / MaxSpeed;
	
	return MaxAnimMoveSpeed * NormalizedSpeed;
}

float UBoss1Anim::CalcDirectionAngle(const ACharacter* Character)
{
	const FVector Front = Character->GetActorForwardVector();
	const FVector Velocity = Character->GetVelocity().GetSafeNormal();

	const float DotFrontAndVelocity = FVector::DotProduct(Front, Velocity);
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(DotFrontAndVelocity));

	const FVector Right = Character->GetActorRightVector();
	const float DotRightAndVelocity = FVector::DotProduct(Right, Velocity);
	const float Sign = FMath::Sign(DotRightAndVelocity);

	return Sign * Angle;
}

void UBoss1Anim::AnimNotify_ShootNeedleAimingEndNotify()
{
	OnShootNeedleAimingEndNotify.Broadcast();
}

void UBoss1Anim::AnimNotify_ThrowMassAimingEndNotify()
{
	OnThrowMassAimingEndNotify.Broadcast();
}

void UBoss1Anim::AnimNotify_EndOnceNotify()
{
	OnEndOnceNotify.Broadcast();
}

void UBoss1Anim::AnimNotify_EndAllNotify()
{
	OnEndAllNotify.Broadcast();
}

void UBoss1Anim::AnimNotify_MeleeAttackStartNotify()
{
	OnMeleeAttackStartNotify.Broadcast();
}

void UBoss1Anim::AnimNotify_MeleeAttackEndNotify()
{
	OnMeleeAttackEndNotify.Broadcast();
}

void UBoss1Anim::AnimNotify_MeleeAttackDelayEndNotify()
{
	OnMeleeAttackDelayEndNotify.Broadcast();
}


