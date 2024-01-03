// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Enemy/EnemyController.h"
#include "Core/Enemy/Enemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Core/MyPlayer.h"
#include "Core/Gadgets/Throwables.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

// Sets default values
AEnemyController::AEnemyController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 3000.f;
	SightConfig->LoseSightRadius = 3500.f;
	SightConfig->PeripheralVisionAngleDegrees = 50.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;
	SightConfig->PointOfViewBackwardOffset = 0.f;
	SightConfig->NearClippingRadius = 0.f;
	SightConfig->SetMaxAge(0.f);

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	HearingConfig->HearingRange = 750.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->SetMaxAge(0.f);

	const auto Perception = AAIController::GetPerceptionComponent();
	Perception->ConfigureSense(*SightConfig);
	Perception->ConfigureSense(*HearingConfig);
	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::TargetPerceptionUpdated);

	MostRecentHologram = nullptr;
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(const AEnemy* const Enemy = Cast<AEnemy>(InPawn))
	{
		if(UBehaviorTree* const Tree = Enemy->GetBehaviorTree())
		{
			UBlackboardComponent* BlackboardComponent;
			UseBlackboard(Tree->BlackboardAsset, BlackboardComponent);
			Blackboard = BlackboardComponent;
			RunBehaviorTree(Tree);
		}
	}
}

void AEnemyController::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	SightPerceptionUpdated(Actor, Stimulus);
	HearingPerceptionUpdated(Stimulus);
}

void AEnemyController::SightPerceptionUpdated(AActor* Actor, const FAIStimulus& Stimulus)
{
	const auto World = GetWorld();
	if(!IsValid(World)) return;

	const auto bSenseIsSight = UAIPerceptionSystem::GetSenseClassForStimulus(World, Stimulus)->IsChildOf(UAISense_Sight::StaticClass());
	
	if(UGameplayStatics::GetPlayerCharacter(World, 0) == Actor && bSenseIsSight)
	{
		const auto Player = Cast<AMyPlayer>(Actor);
		if(!IsValid(Player)) return;

		if(!Player->GetWearingDisguise())
		{
			Blackboard->SetValueAsObject("TargetActor", Player);
		}
		else
		{
			Handle = Player->OnGlassesRemoved.AddLambda([=, this]() -> void
			{
				if(!IsValid(GetPawn())) return;
				auto DirectionToPlayer = GetPawn()->GetActorLocation() - Player->GetActorLocation();
				DirectionToPlayer.Normalize();
				if(LineOfSightTo(Player) && FVector::DotProduct(GetPawn()->GetActorForwardVector(), DirectionToPlayer) < 0)
				{
					Blackboard->SetValueAsObject("TargetActor", Player);
				}
				Player->OnGlassesRemoved.Remove(Handle);
			});
		}
	}
	if(bSenseIsSight && IsValid(Cast<ADecoyHologram>(Actor)) && MostRecentHologram != Actor)
	{
		Blackboard->SetValueAsObject("HologramActor", Actor);
		MostRecentHologram = Actor;
	}
}

void AEnemyController::HearingPerceptionUpdated(const FAIStimulus& Stimulus)
{
	const auto World = GetWorld();
	if(!IsValid(World)) return;

	if(const auto Enemy = Cast<AEnemy>(GetPawn());
		UAIPerceptionSystem::GetSenseClassForStimulus(World, Stimulus)->IsChildOf(UAISense_Hearing::StaticClass())
		&& IsValid(Enemy) && !IsValid(Blackboard->GetValueAsObject("TargetActor")))
	{
		Blackboard->SetValueAsVector("SuspiciousLocation", Stimulus.StimulusLocation);
		Blackboard->SetValueAsBool("IsFocusedOnSuspiciousNoise", true);
	}
}

// Called every frame
void AEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if(OnMovementComplete.IsBound())
	{
		OnMovementComplete.Execute(Result);
		OnMovementComplete.Unbind();
	}
}

