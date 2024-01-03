// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_EnemyChecker.generated.h"

USTRUCT(BlueprintType)
struct FCheckParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Check)
	bool bHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Check)
	bool bPlayerSeen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Check)
	bool bNoisemakerHeard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Check)
	bool bHologramSeen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Check)
	bool bFocusOnSuspiciousNoise;

	FCheckParameters() : bHit(false),
						 bPlayerSeen(false),
						 bNoisemakerHeard(false),
						 bHologramSeen(false),
						 bFocusOnSuspiciousNoise(false) {}
	FCheckParameters(const bool bInHit, const bool bInPlayerSeen, const bool bInNoisemakerHeard, const bool bInHologramSeen, const bool bInFocusOnSuspiciousNoise)
		: bHit(bInHit),
		  bPlayerSeen(bInPlayerSeen),
		  bNoisemakerHeard(bInNoisemakerHeard),
		  bHologramSeen(bInHologramSeen),
		  bFocusOnSuspiciousNoise(bInFocusOnSuspiciousNoise) {}
};

/**
 * 
 */
UCLASS(HideCategories=("Blackboard"))
class PROJECTCATCH_API UBTDecorator_EnemyChecker : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	UBTDecorator_EnemyChecker();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard,
		FBlackboard::FKey ChangedKeyID) override;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Checker, meta=(AllowPrivateAccess = "true"))
	FCheckParameters Params;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Checker, meta=(AllowPrivateAccess = "true"))
	FBlackboardKeySelector IsHitKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Checker, meta=(AllowPrivateAccess = "true"))
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Checker, meta=(AllowPrivateAccess = "true"))
	FBlackboardKeySelector LastHeardThrowableKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Checker, meta=(AllowPrivateAccess = "true"))
	FBlackboardKeySelector HologramActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Checker, meta=(AllowPrivateAccess = "true"))
	FBlackboardKeySelector IsFocusedOnSuspiciousNoiseKey;
};
