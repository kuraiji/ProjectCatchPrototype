// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Enemy/Services/BTService_UpdateEnemyKeys.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Core/Enemy/Enemy.h"
#include "Core/Enemy/EnemyController.h"

UBTService_UpdateEnemyKeys::UBTService_UpdateEnemyKeys()
{
	bNotifyTick = true;
	NodeName = "Update Enemy Keys";
}

void UBTService_UpdateEnemyKeys::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if(!IsValid(Controller)) return;
	const auto Enemy = Cast<AEnemy>(Controller->GetPawn());
	if(!IsValid(Enemy)) return;
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if(!IsValid(Blackboard)) return;

	Blackboard->SetValueAsBool("IsHit", Enemy->GetIsHit());
	Blackboard->SetValueAsObject("LastHeardThrowable", Cast<UObject>(Enemy->GetLastHeardThrowable()));
}