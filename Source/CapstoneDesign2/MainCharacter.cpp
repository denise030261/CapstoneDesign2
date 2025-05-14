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

	AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &AMainCharacter::HandleOnMontageNotifyComponent);
		MontageEndDelegate.BindUObject(this, &AMainCharacter::OnAttackMontageEnded);
		AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttakcMontages[0]);
	}

	if (UIPlayerClass)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		
		if (PC == nullptr)
			return;

		UIPlayerInstance = CreateWidget<UUserWidget>(PC, UIPlayerClass);
		if (UIPlayerInstance)
		{
			UIPlayerInstance->AddToViewport();
			PC->SetInputMode(FInputModeGameOnly());
		}
	}
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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AMainCharacter::Attack);
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

void AMainCharacter::Attack(const FInputActionValue& Value)
{
	if (AnimInst && !bAttack)
	{
		bAttack = true;
		FRotator YawRotation(0, GetActorRotation().Yaw, 0);
		ThrowRotation = YawRotation;

		if (Talisman == nullptr)
			return;

		ATalisman* TalismanObject = NewObject<ATalisman>(GetTransientPackage() /*or owner, or whatever */, *Talisman);

		if(TalismanObject->TalismanDataAsset->SkillInfo.Attribute)
			if (UPassiveSkill* PassiveObjecet = Cast<UPassiveSkill>(TalismanObject->TalismanDataAsset->SkillInfo.Attribute))
			{
				PassiveObjecet->SkillExecute(TalismanObject, GetWorld());
			}

		ThrowTalisman(); // When spawn, Animation
		
		if(TalismanObject)
		{
			if (TalismanObject->TalismanDataAsset->SkillInfo.AnimationType == "Blow")
			{
				AnimInst->Montage_Play(AttakcMontages[1]);
				bSkillEffect = false;
				AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttakcMontages[1]);
			}
			else if (TalismanObject->TalismanDataAsset->SkillInfo.AnimationType == "Throw")
			{
				AnimInst->Montage_Play(AttakcMontages[3]);
				bSkillEffect = false;
				AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttakcMontages[3]);
			}
			else if (TalismanObject->TalismanDataAsset->SkillInfo.AnimationType == "Dance")
			{
				AnimInst->Montage_Play(AttakcMontages[2]);
				bSkillEffect = false;
				AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttakcMontages[2]);
			}
			else
			{
				AnimInst->Montage_Play(AttakcMontages[0]);
				MontageEndDelegate.BindUObject(this, &AMainCharacter::OnAttackMontageEnded);
				AnimInst->Montage_SetEndDelegate(MontageEndDelegate, AttakcMontages[0]);
			}

		}
	}
	else
	{
		AttackComboIndex = 1;
	}
}

void AMainCharacter::HandleOnMontageNotifyComponent(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload)
{ 
	if (CameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShakeClass);
	}
	AttackComboIndex--;

	if (AttackComboIndex < 0)
	{
		if (AnimInst)
		{
			bAttack = false;
			bSkillEffect = false;
			AttackComboIndex = 0;
			//SetActorRotation(ThrowRotation); // Origin Rotation
			AnimInst->Montage_Stop(0.4f, AttakcMontages[0]);
		}
	}

	if (NotifyName == "Spawn" && Talisman != nullptr)
		ThrowTalisman();
}

void AMainCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttakcMontages[0])
	{
		AttackComboIndex = 0;
	}
	bAttack = false;
}

void AMainCharacter::ThrowTalisman()
{
	FVector ForwardDirection = FRotationMatrix(ThrowRotation).GetUnitAxis(EAxis::X);
	FVector ThrowLocation = GetActorLocation() + ForwardDirection * 200;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATalisman* TalismanInstance = GetWorld()->SpawnActor<ATalisman>(Talisman, ThrowLocation, ThrowRotation, SpawnParams);

	if (TalismanInstance == nullptr)
		return;

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
	FRotator RotateRotation(0, -85, 0);
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

