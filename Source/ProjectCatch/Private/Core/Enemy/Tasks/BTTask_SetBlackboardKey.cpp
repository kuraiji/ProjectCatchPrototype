// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Enemy/Tasks/BTTask_SetBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetBlackboardKey::UBTTask_SetBlackboardKey(FObjectInitializer const& ObjectInitializer):
	KeyType(EBlackboardKeyType::EBKT_Bool),
	ObjectValue(nullptr),
	bBooleanValue(false)
{
	NodeName = "Set Blackboard Key";
}

EBTNodeResult::Type UBTTask_SetBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	switch (KeyType)
	{
		case EBlackboardKeyType::EBKT_Bool:
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(KeyName, bBooleanValue);
			break;
	case EBlackboardKeyType::EBKT_Object:
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(KeyName, ObjectValue);
			break;
	}
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_SetBlackboardKey::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}