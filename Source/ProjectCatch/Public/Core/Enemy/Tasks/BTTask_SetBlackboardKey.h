// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetBlackboardKey.generated.h"

UENUM(BlueprintType)
enum class EBlackboardKeyType : uint8
{
	EBKT_Bool UMETA(DisplayName = "Boolean"),
	EBKT_Object UMETA(DisplayName = "Object")
};

/**
 * 
 */
UCLASS()
class PROJECTCATCH_API UBTTask_SetBlackboardKey : public UBTTaskNode
{
	GENERATED_BODY()

public:
	explicit UBTTask_SetBlackboardKey(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blackboard, meta = (AllowPrivateAccess = "true"))
	FName KeyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blackboard, meta = (AllowPrivateAccess = "true"))
	EBlackboardKeyType KeyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blackboard, meta = (AllowPrivateAccess = "true"))
	UObject* ObjectValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blackboard, meta = (AllowPrivateAccess = "true"))
	bool bBooleanValue;
};
