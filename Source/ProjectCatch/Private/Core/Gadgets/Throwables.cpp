// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Gadgets/Throwables.h"
#include "Core/Enemy/Enemy.h"
#include "HelperFunctions.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"


// Sets default values
AThrowables::AThrowables()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DetonateDelay = 3.f;
}

void AThrowables::PinPulled()
{
	const FTimerDelegate ExplosionDelegate = FTimerDelegate::CreateUObject(this, &AThrowables::Detonate);
	FTimerHandle ExplosionHandle;
	GetWorldTimerManager().SetTimer(ExplosionHandle, ExplosionDelegate, DetonateDelay, false);
}

void AThrowables::Destroyed()
{
	Super::Destroyed();
}

void AThrowables::Detonate()
{}

// Called when the game starts or when spawned
void AThrowables::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AThrowables::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ANoiseMaker::ANoiseMaker()
{
	NoiseSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Noise Sphere"));
	NoiseSphere->SetupAttachment(RootComponent);
	NoiseSphere->SetSphereRadius(124.f);
	NoiseSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NoiseSphere->OnComponentBeginOverlap.AddDynamic(this, &ANoiseMaker::OnEnemyEnter);
}

void ANoiseMaker::Detonate()
{
	Super::Detonate();
	NoiseSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FTimerHandle Handle;
	const FTimerDelegate Delegate = FTimerDelegate::CreateLambda([=, this]() -> void
	{
		if(!IsValid(NoiseSphere)) return;
		NoiseSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	});
	GetWorldTimerManager().SetTimer(Handle, Delegate, 5.f, false);
}

void ANoiseMaker::OnEnemyEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
		Enemy->NoiseHeard(this);
	}
}

ADecoy::ADecoy()
{}

void ADecoy::Detonate()
{
	Super::Detonate();
	const FTimerDelegate StopDelegate = FTimerDelegate::CreateLambda([=, this]() -> void
	{
		UStaticMeshComponent* MeshComponent = GetStaticMeshComponent();
		float MeshSize = MeshComponent->GetStaticMesh()->GetBoundingBox().GetSize().Z;
		float Scalar = MeshSize / 2.f + 3.f;
		FHitResult Result;
		FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
		Params.AddIgnoredActor(this);
		UHelperFunctions::SimpleSingleLineTrace(this, Result, GetActorLocation(),
			GetActorLocation() + FVector::DownVector * Scalar,
			ECC_WorldStatic, false, FColor::Red, Params);
		if(!Result.bBlockingHit) return;
		
		ADecoyHologram* HologramActor = Cast<ADecoyHologram>(GetWorld()->SpawnActor(ADecoyHologram::StaticClass()));
		USkeletalMeshComponent* SkeletalMeshComponent = HologramActor->GetMesh();
		SkeletalMeshComponent->SetSkeletalMesh(HologramMesh);
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
		float Offset = SkeletalMeshComponent->GetSkeletalMeshAsset()->GetPhysicsAsset()->CalcAABB(SkeletalMeshComponent, FTransform::Identity).GetSize().Z / 4;
		MeshComponent->SetSimulatePhysics(false);
		HologramActor->SetActorLocation(GetActorLocation() + FVector::UpVector * Offset);
		Hologram = HologramActor;
		
		GetWorldTimerManager().ClearTimer(StopTimer);
	});
	GetWorldTimerManager().SetTimer(StopTimer, StopDelegate, .1, true);
}

void ADecoy::Destroyed()
{
	Super::Destroyed();
	if(Hologram == nullptr) return;
	Hologram->Destroy();
}
