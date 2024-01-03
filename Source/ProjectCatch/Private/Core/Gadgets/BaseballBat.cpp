// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Gadgets/BaseballBat.h"
#include "Components/BoxComponent.h"


// Sets default values
ABaseballBat::ABaseballBat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit Collision"));
	HitCollision->SetupAttachment(RootComponent);
	HitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ABaseballBat::BeginPlay()
{
	Super::BeginPlay();
	
}