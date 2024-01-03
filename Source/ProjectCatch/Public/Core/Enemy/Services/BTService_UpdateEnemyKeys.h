// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateEnemyKeys.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCATCH_API UBTService_UpdateEnemyKeys : public UBTService
{
	GENERATED_BODY()

	UBTService_UpdateEnemyKeys();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
