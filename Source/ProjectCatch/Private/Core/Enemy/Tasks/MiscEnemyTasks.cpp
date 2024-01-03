#include "Core/Enemy/Tasks/MiscEnemyTasks.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Core/Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"

UBTTask_SetRunning::UBTTask_SetRunning(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Set Enemy Running";
	bRunning = false;
}

EBTNodeResult::Type UBTTask_SetRunning::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = OwnerComp.GetAIOwner();
	if(!IsValid(Controller)) return EBTNodeResult::Failed;
	const auto Enemy = Cast<AEnemy>(Controller->GetPawn());
	if(!IsValid(Enemy)) return EBTNodeResult::Failed;

	Enemy->SetRunState(bRunning);
	
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_SetRunning::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

UBTTask_ClearThrowable::UBTTask_ClearThrowable(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Clear Throwable";
}

EBTNodeResult::Type UBTTask_ClearThrowable::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = OwnerComp.GetAIOwner();
	if(!IsValid(Controller)) return EBTNodeResult::Failed;
	const auto Enemy = Cast<AEnemy>(Controller->GetPawn());
	if(!IsValid(Enemy)) return EBTNodeResult::Failed;

	Enemy->GetLastHeardThrowableRef() = nullptr;
	
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_ClearThrowable::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

UBTTask_SetTargetActor::UBTTask_SetTargetActor(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Set Player as Target Actor";
}

EBTNodeResult::Type UBTTask_SetTargetActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if(!IsValid(Blackboard)) return EBTNodeResult::Failed;
	const auto Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	Blackboard->SetValueAsObject("TargetActor", Player);
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_SetTargetActor::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
