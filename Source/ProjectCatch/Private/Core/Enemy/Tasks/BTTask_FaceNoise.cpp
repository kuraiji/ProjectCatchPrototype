// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Enemy/Tasks/BTTask_FaceNoise.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Core/Enemy/Enemy.h"
#include "Core/Enemy/EnemyController.h"

UBTTask_FaceNoise::UBTTask_FaceNoise(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Face Suspicious Noise";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FaceNoise::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if(!IsValid(Controller)) return EBTNodeResult::Failed;
	const auto Enemy = Cast<AEnemy>(Controller->GetPawn());
	if(!IsValid(Enemy)) return EBTNodeResult::Failed;
	
	FBTFaceNoiseMemory* Memory = reinterpret_cast<FBTFaceNoiseMemory*>(NodeMemory);
	Memory->Alpha = 0.f;
	Memory->OriginalDirection = Enemy->GetActorForwardVector().Rotation();
	auto DesiredDirection = OwnerComp.GetBlackboardComponent()->GetValueAsVector("SuspiciousLocation") - Enemy->GetActorLocation();
	DesiredDirection.Normalize();
	Memory->DesiredDirection = DesiredDirection.Rotation();
	Memory->DesiredDirection.Pitch = 0.f;
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_FaceNoise::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

void UBTTask_FaceNoise::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if(!IsValid(Controller)) return;
	const auto Enemy = Cast<AEnemy>(Controller->GetPawn());
	if(!IsValid(Enemy)) return;

	FBTFaceNoiseMemory* Memory = reinterpret_cast<FBTFaceNoiseMemory*>(NodeMemory);
	Memory->Alpha += DeltaSeconds;
	Enemy->SetActorRotation(FMath::Lerp(Memory->OriginalDirection, Memory->DesiredDirection, Memory->Alpha));
	if(Memory->Alpha < 1.f) return;
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

uint16 UBTTask_FaceNoise::GetInstanceMemorySize() const
{
	return sizeof(FBTFaceNoiseMemory);
}
