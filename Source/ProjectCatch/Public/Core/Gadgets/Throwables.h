// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Character.h"
#include "Throwables.generated.h"

class ASkeletalMeshActor;
class USphereComponent;

UCLASS()
class PROJECTCATCH_API AThrowables : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AThrowables();
	virtual void Tick(float DeltaTime) override;
	
	void PinPulled();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Detonate();
	virtual void Destroyed() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Throwable, meta=(AllowPrivateAccess = "true", DisplayName = "Detonate Delay"))
	float DetonateDelay;
};

UCLASS()
class ANoiseMaker : public AThrowables
{
	GENERATED_BODY()

public:
	ANoiseMaker();

protected:
	virtual void Detonate() override;

	UFUNCTION()
	void OnEnemyEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NoiseMaker", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> NoiseSphere;
};

UCLASS()
class ADecoyHologram : public ACharacter
{
	GENERATED_BODY()
};

UCLASS()
class ADecoy : public AThrowables
{
	GENERATED_BODY()

public:
	ADecoy();

protected:
	virtual void Detonate() override;
	virtual void Destroyed() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decoy", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMesh> HologramMesh;

	UPROPERTY()
	TObjectPtr<ADecoyHologram> Hologram;

	FTimerHandle StopTimer;
};