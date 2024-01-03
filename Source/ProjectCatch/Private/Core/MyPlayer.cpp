// Fill out your copyright notice in the Description page of Project Settings.
#include "Core/MyPlayer.h"
#include "Core/Gadgets/Vaccum.h"
#include "HelperFunctions.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/StaticMeshActor.h"
#include "InputActionValue.h"
#include "Core/Gadgets/BaseballBat.h"
#include "Core/Gadgets/Throwables.h"

AMyPlayer::AMyPlayer()
{
 	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 350.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 15.f;
	SpringArm->CameraRotationLagSpeed = 15.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->SetRelativeLocation(FVector(0.f, 0.f, 40.f));

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MinAnalogWalkSpeed = 20.f;
		MovementComponent->BrakingDecelerationWalking = 2000.f;
		MovementComponent->JumpZVelocity = 400.f;
		MovementComponent->BrakingDecelerationFalling = 1500.f;
		MovementComponent->AirControl = 0.15f;
		UHelperFunctions::ConstructEditorRotator(MovementComponent->RotationRate, 0.f, 0.f, 500.f);
		MovementComponent->bOrientRotationToMovement = true;
		DefaultSpeed = MovementComponent->MaxWalkSpeed;
	}

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SneakSpeed = 200.f;
	CurrentGadget = EPlayerGadgets::EPG_None;
	CurrentState = EPlayerStates::EPS_Idle;
}

void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	if (GetWorld())
	{
		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
		{
			CameraManager->ViewPitchMax = 20.f;
			CameraManager->ViewPitchMin = -30.f;
		}
	}
	SetPlayerGadget(EPlayerGadgets::EPG_Melee);
}

void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyPlayer::Jumping);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayer::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPlayer::Look);

		//Walk Hold
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &AMyPlayer::HoldWalk, true);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &AMyPlayer::HoldWalk, false);

		//Gadget Action
		EnhancedInputComponent->BindAction(GadgetAction, ETriggerEvent::Started, this, &AMyPlayer::ActionPressed);
		EnhancedInputComponent->BindAction(GadgetAction, ETriggerEvent::Completed, this, &AMyPlayer::ActionReleased);

		//Gadget Wheel
		EnhancedInputComponent->BindAction(GadgetWheelAction, ETriggerEvent::Started, this, &AMyPlayer::WheelPressed);
		EnhancedInputComponent->BindAction(GadgetWheelAction, ETriggerEvent::Completed, this, &AMyPlayer::WheelReleased);
	}
}

void AMyPlayer::Jumping()
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		if (MovementComponent->IsMovingOnGround())
		{
			JumpMaxCount = 2;
		}
		Jump();
		MovementComponent->bNotifyApex = true;
	}
}

void AMyPlayer::HoldWalk(bool bWalk)
{
	GetCharacterMovement()->MaxWalkSpeed = bWalk ? SneakSpeed : DefaultSpeed;
}

void AMyPlayer::Move(const FInputActionValue& Value)
{
	if (CurrentState == EPlayerStates::EPS_Idle)
	{
		const FRotator ControlRotation = GetControlRotation();
		AddMovementInput(UKismetMathLibrary::GetForwardVector(ControlRotation), Value.Get<FVector2D>().Y);
		AddMovementInput(UKismetMathLibrary::GetRightVector(ControlRotation), Value.Get<FVector2D>().X);
	}
}

void AMyPlayer::Look(const FInputActionValue& Value)
{
	MouseDirection = Value.Get<FVector2D>();
	if(bShowingWidget) return;
	AddControllerYawInput(MouseDirection.X);
	AddControllerPitchInput(MouseDirection.Y);
}

void AMyPlayer::ActionPressed()
{
	bSoft = false;
	bPressingActionButton = true;
	if (CurrentState == EPlayerStates::EPS_Idle && !GetCharacterMovement()->IsFalling() && !bShowingWidget)
	{
		const FTimerDelegate Lambda = FTimerDelegate::CreateLambda([=, this]() -> void
			{
				bUsingAction = true;
				CurrentState = EPlayerStates::EPS_Action;

				//Disguise Logic
				if(CurrentGadget == EPlayerGadgets::EPG_Disguise)
				{
					const FTimerDelegate InnerLambda = FTimerDelegate::CreateLambda([=, this]() -> void
					{
						if(UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(GadgetActor->GetRootComponent()))
						{
							Mesh->SetVisibility(!Mesh->IsVisible());
							if(!Mesh->IsVisible())
							{
								OnGlassesRemoved.Broadcast();
							}
						}
					});
					FTimerHandle Handle;
					GetWorldTimerManager().SetTimer(Handle, InnerLambda, .4f, false);
				}

				//Vaccum Logic
				if(CurrentGadget == EPlayerGadgets::EPG_Vaccum)
				{
					Cast<AVaccum>(GadgetActor)->Sucking();
				}

				//Throwable Logic
				if(CurrentGadget == EPlayerGadgets::EPG_NoiseMaker || CurrentGadget == EPlayerGadgets::EPG_Decoy)
				{
					Cast<AThrowables>(GadgetActor)->PinPulled();
				}
			}
		);
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, Lambda, .1f, false);
	}
	else if(bShowingWidget)
	{
		WheelClick();
	}
}

void AMyPlayer::ActionReleased()
{
	bPressingActionButton = false;
	if(CurrentGadget != EPlayerGadgets::EPG_Vaccum) return;
	Cast<AVaccum>(GadgetActor)->StopSucking();
}

void AMyPlayer::SetPlayerGadget(EPlayerGadgets Gadgets, bool bForce)
{
	if (bForce || Gadgets != EPlayerGadgets::EPG_None && Gadgets != CurrentGadget)
	{
		switch (CurrentGadget)
		{
			case EPlayerGadgets::EPG_Melee:
			case EPlayerGadgets::EPG_Vaccum:
			case EPlayerGadgets::EPG_NoiseMaker:
			case EPlayerGadgets::EPG_Decoy:
				if (IsValid(GadgetActor))
				{
					GadgetActor->Destroy();
				}
				break;
			case EPlayerGadgets::EPG_Disguise:
				if (IsValid(GadgetActor))
				{
					GadgetActor->Destroy();
				}
				OnGlassesRemoved.Broadcast();
				break;
			default:{}
		}
		switch (Gadgets)
		{
			case EPlayerGadgets::EPG_Melee:
				if (ABaseballBat* Gadget = Cast<ABaseballBat>(GetWorld()->SpawnActor(BaseballBatClass)))
				{
					if (UStaticMeshComponent* MeshComponent = Gadget->GetComponentByClass<UStaticMeshComponent>())
					{
						MeshComponent->SetMobility(EComponentMobility::Movable);
						//MeshComponent->SetStaticMesh(BatGadgetMesh);
						MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						const FAttachmentTransformRules Attachment(EAttachmentRule::KeepRelative, true);
						Gadget->AttachToComponent(GetMesh(), Attachment, TEXT("Grip"));
						GadgetActor = Cast<AActor>(Gadget);
					}
				}
				break;
			case EPlayerGadgets::EPG_Vaccum:
				if (AVaccum* Gadget = Cast<AVaccum>(GetWorld()->SpawnActor(VaccumClass)))
				{
					FRotator Rotator;
					const FAttachmentTransformRules Attachment(EAttachmentRule::KeepRelative, true);
					Gadget->GetBackpack()->AttachToComponent(GetMesh(), Attachment, TEXT("Back"));
					Gadget->GetBackpack()->SetRelativeLocation(FVector(-40.f, -25.f, 0.f));
					UHelperFunctions::ConstructEditorRotator(Rotator, 0.f, 90.f, 180.f);
					Gadget->GetBackpack()->SetRelativeRotation(Rotator);
					Gadget->GetNozzleGrip()->AttachToComponent(GetMesh(), Attachment, TEXT("Grip"));
					Gadget->GetNozzleGrip()->SetRelativeLocation(FVector::ZeroVector);
					UHelperFunctions::ConstructEditorRotator(Rotator, -90.f, 180.f, 0.f);
					Gadget->GetNozzleGrip()->SetRelativeRotation(Rotator);
					GadgetActor = Cast<AActor>(Gadget);
				}
				break;
			case EPlayerGadgets::EPG_NoiseMaker:
			case EPlayerGadgets::EPG_Decoy:
				{
					const TSubclassOf<AThrowables> Class = Gadgets == EPlayerGadgets::EPG_NoiseMaker ? NoiseMakerClass : DecoyClass;
					if(AThrowables* Gadget = Cast<AThrowables>(GetWorld()->SpawnActor(Class)))
					{
						if (UStaticMeshComponent* MeshComponent = Gadget->GetComponentByClass<UStaticMeshComponent>())
						{
							MeshComponent->SetMobility(EComponentMobility::Movable);
							MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
							const FAttachmentTransformRules Attachment(EAttachmentRule::KeepRelative, true);
							Gadget->AttachToComponent(GetMesh(), Attachment, TEXT("Grip"));
							MeshComponent->SetRelativeLocation(FVector(-2.f, 2.f, -2.5f));
							GadgetActor = Cast<AActor>(Gadget);
						}
					}
				}
				break;
			case EPlayerGadgets::EPG_Disguise:
				if(AStaticMeshActor* Gadget = Cast<AStaticMeshActor>(GetWorld()->SpawnActor(AStaticMeshActor::StaticClass())))
				{
					if(UStaticMeshComponent* MeshComponent = Gadget->GetComponentByClass<UStaticMeshComponent>())
					{
						FRotator Rotator;
						MeshComponent->SetMobility(EComponentMobility::Movable);
						MeshComponent->SetStaticMesh(DisguiseGadgetMesh);
						MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						const FAttachmentTransformRules Attachment(EAttachmentRule::KeepRelative, true);
						Gadget->AttachToComponent(GetMesh(), Attachment, TEXT("Face"));
						UHelperFunctions::ConstructEditorRotator(Rotator, 90, -90.f, 0);
						MeshComponent->SetRelativeLocation(FVector(2.f, -4.5f, 0.f));
						MeshComponent->SetRelativeRotation(Rotator);
						MeshComponent->SetVisibility(false);
						GadgetActor = Cast<AActor>(Gadget);
					}
				}
			default: {}
		}
		CurrentGadget = Gadgets;
	}
}

void AMyPlayer::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	const FTimerDelegate Lambda = FTimerDelegate::CreateLambda([=, this]() -> void
			{
				JumpMaxCount = 0;
			}
		);
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, Lambda, .2f, false);
}

void AMyPlayer::ThrowBall()
{
	if(UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(GadgetActor->GetRootComponent()))
	{
		const FDetachmentTransformRules Detachment(EDetachmentRule::KeepWorld, false);
		GadgetActor->DetachFromActor(Detachment);
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		const FVector Impulse = bSoft ?
			(GetActorForwardVector() + GetActorUpVector()) * FVector(100.f) :
			GetActorForwardVector() * FVector(250.f);
		MeshComponent->AddImpulse(Impulse);
		if(IsValid(ActiveBall))
		{
			ActiveBall->Destroy();
		}
		ActiveBall = *GadgetActor;
		GadgetActor = nullptr;
		SetPlayerGadget(CurrentGadget, true);
	}
}

bool AMyPlayer::GetWearingDisguise() const
{
	return CurrentGadget == EPlayerGadgets::EPG_Disguise && Cast<AStaticMeshActor>(GadgetActor)->GetStaticMeshComponent()->IsVisible();
}
