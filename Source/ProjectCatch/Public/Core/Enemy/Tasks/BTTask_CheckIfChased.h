// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CheckIfChased.generated.h"

struct FBTCheckIfChasedMemory
{
	float Alpha = 0.f;
	FRotator OriginalDirection = FRotator::ZeroRotator;
	FRotator DesiredDirection = FRotator::ZeroRotator;
};

/**
 * 
 */
UCLASS()
class PROJECTCATCH_API UBTTask_CheckIfChased : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	explicit UBTTask_CheckIfChased(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;
	
protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
