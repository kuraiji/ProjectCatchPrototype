// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Enemy/Enemy.h"
#include "Core/Enemy/EnemyController.h"
#include "Core/Gadgets/BaseballBat.h"
#include "Math/UnrealMathVectorCommon.h" 
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyController::StaticClass();
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	Alpha = 0.f;
	DefaultSpeed = 250.f;
	RunSpeed = 550.f;
	EnemyType = EEnemyType::EET_WalkAround;
	bHit = false;

	
	OnActorBeginOverlap.AddDynamic(this, &AEnemy::ActorBeginOverlap);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	OriginalTransform = GetActorTransform();
}

void AEnemy::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if(Cast<ABaseballBat>(OtherActor) && !bHit)
	{
		bHit = true;
		FTimerHandle Handle;
		const FTimerDelegate Delegate = FTimerDelegate::CreateLambda([=, this]() -> void
		{
			bHit = false;
		});
		GetWorldTimerManager().SetTimer(Handle, Delegate, 5.f, false);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::Sucked(const FVector& SuckLocation)
{
	const FVector StartLocation = GetActorLocation();
	const FRotator StartRotation = GetActorRotation();
	const FRotator EndRotation = (SuckLocation - StartLocation).GetSafeNormal().Rotation().GetInverse();
	GetController()->Destroy();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	const FTimerDelegate SuckDelegate = FTimerDelegate::CreateLambda([=, this] () -> void
	{
		SetActorTransform(FTransform(
			FMath::Lerp(StartRotation, EndRotation, Alpha),
			FMath::Lerp(StartLocation, SuckLocation, Alpha),
			FMath::Lerp(FVector::One(), FVector::Zero(), Alpha)));
		Alpha += .01f;
		if(Alpha < 1.f) return;
		GetWorldTimerManager().ClearTimer(SuckHandle);
		Destroy();
	});
	GetWorldTimerManager().SetTimer(SuckHandle, SuckDelegate, .01, true);
}

void AEnemy::NoiseHeard(AThrowables* Throwable)
{
	if(LastHeardThrowable == Throwable) return;
	LastHeardThrowable = Throwable;
}

void AEnemy::SetRunState(const bool bRun) const
{
	GetCharacterMovement()->MaxWalkSpeed = bRun ? RunSpeed : DefaultSpeed;
}
