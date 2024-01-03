// Fill out your copyright notice in the Description page of Project Settings.
#include "Core/Gadgets/Vaccum.h"
#include "Core/Enemy/Enemy.h"
#include "CableComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
AVaccum::AVaccum()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Outer = CreateDefaultSubobject<USceneComponent>(TEXT("Outer"));
	RootComponent = Outer;

	Backpack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Backpack"));
	Backpack->SetupAttachment(RootComponent);
	Backpack->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Cable = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	Cable->SetupAttachment(Backpack);

	NozzleGrip = CreateDefaultSubobject<USceneComponent>(TEXT("NozzleGrip"));
	NozzleGrip->SetupAttachment(RootComponent);

	Nozzle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Nozzle"));
	Nozzle->SetupAttachment(NozzleGrip);
	Nozzle->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NozzleHole = CreateDefaultSubobject<USceneComponent>(TEXT("NozzleHole"));
	NozzleHole->SetupAttachment(Nozzle);

	NozzleFront = CreateDefaultSubobject<USceneComponent>(TEXT("Nozzle Front"));
	NozzleFront->SetupAttachment(Nozzle);

	LineTraceScaler = 5.f;
	SuckingDelay = 2.f;
	BoxHalfSize = 5.f;
}

void AVaccum::Sucking()
{
	const FTimerDelegate SuckDelegate = FTimerDelegate::CreateLambda([=, this]() -> void
		{
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), Nozzle->GetComponentLocation(), 1.f, this, 0.f);
			const TArray<AActor*> IgnoredActors;
			if(FHitResult Result;
				UKismetSystemLibrary::BoxTraceSingle(GetWorld(), NozzleFront->GetComponentLocation(),
					NozzleFront->GetComponentLocation() + NozzleFront->GetForwardVector() * LineTraceScaler, FVector(BoxHalfSize),
					FRotator::ZeroRotator, UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, IgnoredActors,
					EDrawDebugTrace::None, Result, true)
					)
			{
				if(const TObjectPtr<AEnemy> TracedEnemy = Cast<AEnemy>(Result.Component->GetOwner()))
				{
					if(SuckingActor == nullptr || SuckingActor != TracedEnemy)
					{
						SuckingActor = TracedEnemy;
						return;
					}
					TracedEnemy->Sucked(NozzleFront->GetComponentLocation());
					OnEnemySuck(TracedEnemy);
				}
			}
			SuckingActor = nullptr;
		}
	);
	GetWorldTimerManager().SetTimer(SuckHandle, SuckDelegate, SuckingDelay, true, .5f);
}

void AVaccum::StopSucking()
{
	GetWorldTimerManager().ClearTimer(SuckHandle);
}
