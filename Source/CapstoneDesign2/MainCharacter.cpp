// Copyright Epic Games, Inc. All Rights Reserved.


#include "MainCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Talisman/PassiveSkill.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "GameFramework/PlayerController.h"


//////////////////////////////////////////////////////////////////////////
// ACapstoneDesign2Character

AMainCharacter::AMainCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMainCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	bNoDamage = false;
	InitCharacterHP();
	InitTalismanState();
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;

	AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &AMainCharacter::HandleOnMontageNotifyComponent);
		MontageEndDelegate.BindUObject(this, &AMainCharacter::OnAttackMontageEnded);
		AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttackMontages[0]);
	}

	if (UIPlayerClass)
	{
		PC = GetWorld()->GetFirstPlayerController();
		PC->bShowMouseCursor = false;

		if (PC == nullptr)
			return;

		UIPlayerInstance = CreateWidget<UUserWidget>(PC, UIPlayerClass);
		if (UIPlayerInstance)
		{
			UIPlayerInstance->AddToViewport();
			PC->SetInputMode(FInputModeGameOnly());
		}
	}

	SetActorTickEnabled(false);
}

void AMainCharacter::InitTalismanState()
{
	CurAttribute = "Normal";
	SelectedTalismanDataAsset = NormalTalismanAssets[0];
	NormalTalismanTimeHandler.SetNum(NormalTalismanAssets.Num());
	bNormalTalismanUses.Init(true, NormalTalismanAssets.Num());
	FireTalismanTimeHandler.SetNum(FireTalismanAssets.Num());
	bFireTalismanUses.Init(true, FireTalismanAssets.Num());
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);

		// Attack
		EnhancedInputComponent->BindAction(CloseAttackAction, ETriggerEvent::Started, this, &AMainCharacter::CloseAttack);
		EnhancedInputComponent->BindAction(RangedAttackAction, ETriggerEvent::Started, this, &AMainCharacter::RangedAttack);
		EnhancedInputComponent->BindAction(MoveAttackAction, ETriggerEvent::Started, this, &AMainCharacter::MoveAttack);
		EnhancedInputComponent->BindAction(BallAttackAction, ETriggerEvent::Started, this, &AMainCharacter::BallAttack);
		EnhancedInputComponent->BindAction(SpecialAttackAction, ETriggerEvent::Started, this, &AMainCharacter::SpecialAttack);
		EnhancedInputComponent->BindAction(GodAttackAction, ETriggerEvent::Started, this, &AMainCharacter::GodAttack);
	}

}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMainCharacter::CloseAttack(const FInputActionValue& Value)
{
	SelectedAttackIndex = 0;
	Attack();
}

void AMainCharacter::RangedAttack(const FInputActionValue& Value)
{
	if(!bAttacking)
		DisableInput(PC);

	SelectedAttackIndex = 1;
	Attack();
}

void AMainCharacter::MoveAttack(const FInputActionValue& Value)
{
	if (!bAttacking)
		DisableInput(PC);
	 
	SelectedAttackIndex = 2;
	Attack();
}

void AMainCharacter::BallAttack(const FInputActionValue& Value)
{
	if (!bAttacking)
		DisableInput(PC);

	SelectedAttackIndex = 3;
	Attack();
}

void AMainCharacter::SpecialAttack(const FInputActionValue& Value)
{
	if (!bAttacking)
		DisableInput(PC);

	SelectedAttackIndex = 4;
	Attack();
}

void AMainCharacter::GodAttack(const FInputActionValue& Value)
{
	if (!bAttacking)
		DisableInput(PC);

	SelectedAttackIndex = 5;
	Attack();
}

void AMainCharacter::Attack()
{
	if (!bAttackEnable())
		return;

	if (AnimInst && !bAttacking)
	{
		bAttacking = true;
		FRotator YawRotation(0, GetActorRotation().Yaw, 0);
		ThrowRotation = YawRotation;

		if (Talisman == nullptr)
			return;

		ATalisman* TalismanObject = NewObject<ATalisman>(GetTransientPackage() /*or owner, or whatever */, *Talisman);
		TalismanObject->TalismanDataAsset = SelectedTalismanDataAsset;

		if(TalismanObject->TalismanDataAsset->SkillInfo.Attribute)
			if (UPassiveSkill* PassiveObjecet = Cast<UPassiveSkill>(TalismanObject->TalismanDataAsset->SkillInfo.Attribute))
			{
				PassiveObjecet->SkillExecute(TalismanObject, GetWorld());
			}

		ThrowTalisman(); // Spawn Talisman
		PlayAttackAnimation(TalismanObject); // Play animation about Talisman
		
		if (SkillForceFeedback)
			GetWorld()->GetFirstPlayerController()->ClientPlayForceFeedback(SkillForceFeedback);
	}
	else
	{
		AttackComboIndex = 1;
	}
}

void AMainCharacter::PlayAttackAnimation(ATalisman* TalismanObject)
{
	if (TalismanObject)
	{
		if (TalismanObject->TalismanDataAsset->SkillInfo.AnimationType == "Blow")
		{
			AnimInst->Montage_Play(AttackMontages[1]);
			bSkillEffect = false;
			AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttackMontages[1]);
		}
		else if (TalismanObject->TalismanDataAsset->SkillInfo.AnimationType == "Throw")
		{
			AnimInst->Montage_Play(AttackMontages[3]);
			bSkillEffect = false;
			AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttackMontages[3]);
		}
		else if (TalismanObject->TalismanDataAsset->SkillInfo.AnimationType == "Dance")
		{
			AnimInst->Montage_Play(AttackMontages[2]);
			bSkillEffect = false;
			AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttackMontages[2]);
		}
		else
		{
			AnimInst->Montage_Play(AttackMontages[0]);
			MontageEndDelegate.BindUObject(this, &AMainCharacter::OnAttackMontageEnded);
			AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttackMontages[0]);
		}
	}
}

bool AMainCharacter::bAttackEnable()
{
	if (CurAttribute == "Normal")
	{
		if (SelectedAttackIndex >= 4)
		{
			EnableInput(PC);
			return false;
		}

		if (NormalTalismanAssets[SelectedAttackIndex])
		{
			SelectedTalismanDataAsset = NormalTalismanAssets[SelectedAttackIndex];
			if (!bNormalTalismanUses[SelectedAttackIndex])
			{
				UE_LOG(LogTemp, Warning, TEXT("CoolTime"));
				EnableInput(PC);
				return false;
			}
			else
			{
				bNormalTalismanUses[SelectedAttackIndex] = false;

				if(SelectedTalismanDataAsset->SkillInfo.CoolTime!=0)
					GetWorld()->GetTimerManager().SetTimer(
						NormalTalismanTimeHandler[SelectedAttackIndex],
						FTimerDelegate::CreateUObject(this, &AMainCharacter::OnSkillCooldownFinished, 0, SelectedAttackIndex),
						SelectedTalismanDataAsset->SkillInfo.CoolTime,
						false
					);
				else
					bNormalTalismanUses[SelectedAttackIndex] = true;
			}
		}
	}
	else
	{
		if (FireTalismanAssets[SelectedAttackIndex])
		{
			SelectedTalismanDataAsset = FireTalismanAssets[SelectedAttackIndex];
			if (!bFireTalismanUses[SelectedAttackIndex])
			{
				UE_LOG(LogTemp, Warning, TEXT("CoolTime"));
				EnableInput(PC);
				return false;
			}
			else
			{
				bFireTalismanUses[SelectedAttackIndex] = false;
				if (SelectedTalismanDataAsset->SkillInfo.CoolTime != 0)
					GetWorld()->GetTimerManager().SetTimer(
						FireTalismanTimeHandler[SelectedAttackIndex],
						FTimerDelegate::CreateUObject(this, &AMainCharacter::OnSkillCooldownFinished, 1, SelectedAttackIndex),
						SelectedTalismanDataAsset->SkillInfo.CoolTime,
						false
					);
				else
					bFireTalismanUses[SelectedAttackIndex] = true;
			}
		}
	}



	return true;
}

void AMainCharacter::ThrowTalisman()
{
	FVector ForwardDirection = FRotationMatrix(ThrowRotation).GetUnitAxis(EAxis::X);
	FVector ThrowLocation = GetActorLocation() + ForwardDirection * 20;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	ATalisman* TalismanInstance = GetWorld()->SpawnActor<ATalisman>(Talisman, ThrowLocation, ThrowRotation + FRotator(0, 90, 0), SpawnParams);

	if (TalismanInstance == nullptr)
		return;

	if (SelectedTalismanDataAsset != nullptr)
		TalismanInstance->TalismanDataAsset = SelectedTalismanDataAsset;

	TalismanInstance->SetMoveDistance(ThrowLocation + ForwardDirection * TalismanInstance->TalismanDataAsset->SkillInfo.Distance);

	// Call SkillExecute Function
	if (Talisman && TalismanInstance->TalismanDataAsset && TalismanInstance->TalismanDataAsset->SkillInfo.Skill)
	{
		UTalismanSkillStrategy* Executor = NewObject<UTalismanSkillStrategy>(
			Talisman,
			TalismanInstance->TalismanDataAsset->SkillInfo.Skill
		);

		if (Executor)
		{
			Executor->SkillExecute(TalismanInstance, GetWorld());  
		}
	}

	// Talisman Fly Effect
	FRotator RotateRotation(0, 0, 180);
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		TalismanInstance->TalismanDataAsset->SkillInfo.Effect,
		TalismanInstance->GetRootComponent(),   
		NAME_None,                              
		FVector::ZeroVector,                   
		RotateRotation,                 
		EAttachLocation::KeepRelativeOffset,   
		true                                    
	);

	// SkillEffect Spawn
	if (!bSkillEffect)
	{
		UNiagaraComponent* NiagaraSpawn = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TalismanInstance->TalismanDataAsset->SkillInfo.SkillEffect,
			GetRootComponent(),   
			NAME_None,                              
			FVector(0, 0, -95),                    
			FRotator(0, 0, 0),                 
			EAttachLocation::KeepRelativeOffset,   
			true                                   
		);

		bSkillEffect = true;
	}

	if (!NiagaraComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn and attach Niagara effect"));
	}
}

void AMainCharacter::Dancing()
{
	AnimInst->Montage_Play(AttackMontages[2]);
	bSkillEffect = false;
	AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttackMontages[2]);
}

void AMainCharacter::SetCharacterHP(int Num)
{
	HP += Num;

	if (Num > 0)
	{
		//Heal
		return;
	}

	if (bNoDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Damage State"));
		return;
	}

	float Stun = 0;
	DisableInput(PC);
	if (HP <= 0)
	{
		DamageLevel = 3;
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly());
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		Stun = 5.7; // Die Animation Time
	}
	else if (Num < 0 && Num >= DamageArea[0])
	{
		DamageLevel = 0;
		Stun = 0.63;
	} // Small React
	else if (Num < DamageArea[0] && Num >= DamageArea[1])
	{
		DamageLevel = 1;
		Stun = 1.03;
	} // Large React
	else if (Num < DamageArea[1])
	{
		DamageLevel = 2;
		Stun = 6;
	} // Fall Down

	if (DamageMontages[DamageLevel])
	{
		AnimInst->Montage_Play(DamageMontages[DamageLevel]);
	}
	bNoDamage = true;
	bAttacking = true;

	// Stun Time
	FTimerHandle StunTimerHandle;
	FTimerDelegate StunDelegate;
	StunDelegate.BindUObject(this, &AMainCharacter::EnableMovement);
	GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, StunDelegate, Stun, false);
	
	if (HitForceFeedback)
		GetWorld()->GetFirstPlayerController()->ClientPlayForceFeedback(HitForceFeedback);
}

void AMainCharacter::EnableMovement()
{
	if (DamageLevel==2)
	{
		AnimInst->Montage_Play(GetupMontage);
		return;
	} // Bigest Damage
	else if (DamageLevel == 3)
	{
		DieWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), DieWidgetClass);
		if (DieWidget)
		{
			DieWidget->AddToViewport();
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(DieWidget->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			GetWorld()->GetFirstPlayerController()->SetInputMode(InputModeData);
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		} 
	} // Die

	// No Damage Time
	AddNoDamage();
	EnableInput(GetWorld()->GetFirstPlayerController());
	bAttacking = false;
}

void AMainCharacter::AddNoDamage()
{
	FTimerDelegate NoDamageDelegate;

	NoDamageDelegate = FTimerDelegate::CreateLambda([this]()
		{
			UE_LOG(LogTemp, Error, TEXT("No Damage Time"));
			bNoDamage = false;
		});

	GetWorld()->GetTimerManager().SetTimer(NoDamageTimer, NoDamageDelegate, NoDamageTime, false);
}

void AMainCharacter::OnSkillCooldownFinished(int32 TalismanAttribute, int32 SkillIndex)
{
	if (TalismanAttribute==0)
	{
		bNormalTalismanUses[SkillIndex] = true;
	}
	else
	{
		bFireTalismanUses[SkillIndex] = true;
	}
}


/////////////////////
//Animation

void AMainCharacter::HandleOnMontageNotifyComponent(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload)
{
	if (NotifyName == "ReleaseNoDamage")
	{
		bAttacking = false;
		EnableInput(PC);
		AddNoDamage();
		return;
	}

	if (CameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShakeClass);
	}
	AttackComboIndex--;

	if (AttackComboIndex < 0)
	{
		if (AnimInst)
		{
			bAttacking = false;
			bSkillEffect = false;
			AttackComboIndex = 0;
			//SetActorRotation(ThrowRotation); // Origin Rotation
			AnimInst->Montage_Stop(0.4f, AttackMontages[0]);
		}
	}

	if (NotifyName == "Spawn" && Talisman != nullptr)
		ThrowTalisman();
}

void AMainCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontages[0])
	{
		AttackComboIndex = 0;
	}
	bAttacking = false;
	bNoDamage = false;
	EnableInput(PC);
}

/*
void AISeeMeCharacter::Focus()
{
	if (AISMPlayerController* ISMPlayerController = GetController<AISMPlayerController>())
	{
		if (ACharacter* OtherCharacter = ISMPlayerController->GetOtherCharacter())
		{
			FocusStartRotator = ISMPlayerController->GetControlRotation();

			float NewYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), OtherCharacter->GetActorLocation()).Yaw;
			FocusEndRotator = FRotator(FocusStartRotator.Pitch, NewYaw, FocusStartRotator.Roll);

			FocusTimeline->PlayFromStart();
		}
	}
}*/