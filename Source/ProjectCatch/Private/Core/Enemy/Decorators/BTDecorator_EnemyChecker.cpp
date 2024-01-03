// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Enemy/Decorators/BTDecorator_EnemyChecker.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Core/MyPlayer.h"
#include "Core/Gadgets/Throwables.h"

UBTDecorator_EnemyChecker::UBTDecorator_EnemyChecker()
{
	NodeName = "Enemy Checker";
	Params = FCheckParameters();

	INIT_DECORATOR_NODE_NOTIFY_FLAGS();

	IsHitKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_EnemyChecker, IsHitKey));
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_EnemyChecker, TargetActorKey), AMyPlayer::StaticClass());
	LastHeardThrowableKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_EnemyChecker, LastHeardThrowableKey), ANoiseMaker::StaticClass());
	HologramActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_EnemyChecker, HologramActorKey), ADecoyHologram::StaticClass());
	IsFocusedOnSuspiciousNoiseKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_EnemyChecker, IsFocusedOnSuspiciousNoiseKey));
}

bool UBTDecorator_EnemyChecker::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if(!IsValid(Blackboard)) return false;
	
	const bool bHit = Blackboard->GetValue<UBlackboardKeyType_Bool>(IsHitKey.GetSelectedKeyID()) && Params.bHit;
	const bool bSeen = IsValid(Blackboard->GetValue<UBlackboardKeyType_Object>(TargetActorKey.GetSelectedKeyID())) && Params.bPlayerSeen;
	const bool bHeard = IsValid(Blackboard->GetValue<UBlackboardKeyType_Object>(LastHeardThrowableKey.GetSelectedKeyID())) && Params.bNoisemakerHeard;
	const bool bHologram = IsValid(Blackboard->GetValue<UBlackboardKeyType_Object>(HologramActorKey.GetSelectedKeyID())) && Params.bHologramSeen;
	const bool bNoise = Blackboard->GetValue<UBlackboardKeyType_Bool>(IsFocusedOnSuspiciousNoiseKey.GetSelectedKeyID()) && Params.bFocusOnSuspiciousNoise;
	
	return !(bHit || bSeen || bHeard || bHologram || bNoise);
}

void UBTDecorator_EnemyChecker::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if(const UBlackboardData* BBAsset = GetBlackboardAsset(); ensure(BBAsset))
	{
		IsHitKey.ResolveSelectedKey(*BBAsset);
		TargetActorKey.ResolveSelectedKey(*BBAsset);
		LastHeardThrowableKey.ResolveSelectedKey(*BBAsset);
		HologramActorKey.ResolveSelectedKey(*BBAsset);
		IsFocusedOnSuspiciousNoiseKey.ResolveSelectedKey(*BBAsset);
	}
}

EBlackboardNotificationResult UBTDecorator_EnemyChecker::OnBlackboardKeyValueChange(
	const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = static_cast<UBehaviorTreeComponent*>(Blackboard.GetBrainComponent());
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (IsHitKey.GetSelectedKeyID() == ChangedKeyID || TargetActorKey.GetSelectedKeyID() == ChangedKeyID
			|| LastHeardThrowableKey.GetSelectedKeyID() == ChangedKeyID || HologramActorKey.GetSelectedKeyID() == ChangedKeyID
			|| IsFocusedOnSuspiciousNoiseKey.GetSelectedKeyID() == ChangedKeyID)
	{
		BehaviorComp->RequestExecution(this);		
	}
	return EBlackboardNotificationResult::ContinueObserving;
}

void UBTDecorator_EnemyChecker::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		const auto HitKeyID = IsHitKey.GetSelectedKeyID();
		const auto TargetKeyID = TargetActorKey.GetSelectedKeyID();
		const auto ThrowableKeyID = LastHeardThrowableKey.GetSelectedKeyID();
		const auto HologramKeyID = HologramActorKey.GetSelectedKeyID();
		const auto SuspiciousKeyID = IsFocusedOnSuspiciousNoiseKey.GetSelectedKeyID();
		BlackboardComp->RegisterObserver(HitKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UBTDecorator_EnemyChecker::OnBlackboardKeyValueChange));
		BlackboardComp->RegisterObserver(TargetKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UBTDecorator_EnemyChecker::OnBlackboardKeyValueChange));
		BlackboardComp->RegisterObserver(ThrowableKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UBTDecorator_EnemyChecker::OnBlackboardKeyValueChange));
		BlackboardComp->RegisterObserver(HologramKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UBTDecorator_EnemyChecker::OnBlackboardKeyValueChange));
		BlackboardComp->RegisterObserver(SuspiciousKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UBTDecorator_EnemyChecker::OnBlackboardKeyValueChange));
	}
}
