// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "EnemyController.generated.h"

struct FAIStimulus;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

DECLARE_DELEGATE_OneParam(FOnMovementCompleteSignature, const FPathFollowingResult&);

UCLASS()
class PROJECTCATCH_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemyController();
	virtual void Tick(float DeltaTime) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	FOnMovementCompleteSignature OnMovementComplete;

protected:
	// Called when the game starts or when spawned
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void SightPerceptionUpdated(AActor* Actor, const FAIStimulus& Stimulus);
	void HearingPerceptionUpdated(const FAIStimulus& Stimulus);

private:
	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY()
	AActor* MostRecentHologram;

	FDelegateHandle Handle;
};
