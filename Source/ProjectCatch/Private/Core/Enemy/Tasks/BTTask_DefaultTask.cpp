// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Enemy/Tasks/BTTask_DefaultTask.h"
#include "Core/Enemy/EnemyController.h"
#include "Core/Enemy/Enemy.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_DefaultTask::UBTTask_DefaultTask(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Default Task";
	bCreateNodeInstance = true;
	Alpha = 0.f;
}

EBTNodeResult::Type UBTTask_DefaultTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(AEnemyController* const Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner()))
	{
		if(AEnemy* Enemy = Cast<AEnemy>(Controller->GetPawn()))
		{
			Enemy->SetRunState(false);
			switch (Enemy->GetEnemyType())
			{
				case EEnemyType::EET_WalkAround:
				{
					if(const auto NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld()))
					{
						if(FNavLocation RandomLocation;
							NavSystem->GetRandomReachablePointInRadius(Enemy->GetActorLocation(), Radius, RandomLocation)
							)
						{
							Controller->OnMovementComplete.BindLambda([=, &OwnerComp, this](const FPathFollowingResult& Result) -> void
							{
								const auto NodeResult = Result.Code == EPathFollowingResult::Success ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
								FinishLatentTask(OwnerComp, NodeResult);
							});
							Controller->MoveToLocation(RandomLocation, AcceptanceRadius);
							return EBTNodeResult::InProgress;
						}
					}
					break;
				}
				case EEnemyType::EET_StandStill:
				{
					if(!Enemy->GetActorLocation().Equals(Enemy->GetOriginalTransform().GetLocation(), 0.1f))
					{
						Controller->OnMovementComplete.BindLambda([=, &OwnerComp, this](const FPathFollowingResult& Result) -> void
						{
							const auto NodeResult = Result.Code == EPathFollowingResult::Success ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
							FinishLatentTask(OwnerComp, NodeResult);
						});
						Controller->MoveToLocation(Enemy->GetOriginalTransform().GetLocation(), 0.f, false);
						return EBTNodeResult::InProgress;
					}
					if(!Enemy->GetActorRotation().Equals(Enemy->GetOriginalTransform().Rotator(), 0.01f))
					{
						const FRotator TempRotation = Enemy->GetActorRotation();
						Alpha = 0.f;
						const FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([=, &OwnerComp, this]() -> void
						{
							Alpha += 0.02f;
							Enemy->SetActorRotation(FMath::Lerp(TempRotation, Enemy->GetOriginalTransform().Rotator(), Alpha));
							if(Alpha < 1.f) return;
							GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
							FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
						});
						GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, .01f, true);
						return EBTNodeResult::InProgress;
					}
					const FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([=, &OwnerComp, this]() -> void
					{
						FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					});
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 5.f, false);
					return EBTNodeResult::InProgress;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_DefaultTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(const auto Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner()))
	{
		Controller->StopMovement();
	}
	FinishLatentAbort(OwnerComp);
	return EBTNodeResult::Aborted;
}
