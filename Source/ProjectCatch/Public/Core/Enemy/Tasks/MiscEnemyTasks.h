#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MiscEnemyTasks.generated.h"

UCLASS()
class PROJECTCATCH_API UBTTask_SetRunning : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UBTTask_SetRunning(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Running, meta = (AllowPrivateAccess = "true"))
	bool bRunning;
};

UCLASS()
class PROJECTCATCH_API UBTTask_ClearThrowable : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UBTTask_ClearThrowable(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS()
class UBTTask_SetTargetActor : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UBTTask_SetTargetActor(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
