// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Enemy/Tasks/BTTask_CheckIfChased.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Core/Enemy/Enemy.h"
#include "Core/Enemy/EnemyController.h"

UBTTask_CheckIfChased::UBTTask_CheckIfChased(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Check If Chased";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_CheckIfChased::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if(!IsValid(Controller)) return EBTNodeResult::Failed;
	const auto Enemy = Cast<AEnemy>(Controller->GetPawn());
	if(!IsValid(Enemy)) return EBTNodeResult::Failed;

	FBTCheckIfChasedMemory* Memory = reinterpret_cast<FBTCheckIfChasedMemory*>(NodeMemory);
	Memory->Alpha = 0.f;
	Memory->OriginalDirection = Enemy->GetActorForwardVector().Rotation();
	Memory->DesiredDirection = (Enemy->GetActorForwardVector() * -1).Rotation();
	OwnerComp.GetBlackboardComponent()->SetValueAsObject("TargetActor", nullptr);
	return EBTNodeResult::InProgress;
}

uint16 UBTTask_CheckIfChased::GetInstanceMemorySize() const
{
	return sizeof(FBTCheckIfChasedMemory);
}

EBTNodeResult::Type UBTTask_CheckIfChased::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

void UBTTask_CheckIfChased::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if(!IsValid(Controller)) return;
	const auto Enemy = Cast<AEnemy>(Controller->GetPawn());
	if(!IsValid(Enemy)) return;

	FBTCheckIfChasedMemory* Memory = reinterpret_cast<FBTCheckIfChasedMemory*>(NodeMemory);
	Memory->Alpha += DeltaSeconds;
	Enemy->SetActorRotation(FMath::Lerp(Memory->OriginalDirection, Memory->DesiredDirection, Memory->Alpha));
	if(Memory->Alpha < 1.f) return;
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
