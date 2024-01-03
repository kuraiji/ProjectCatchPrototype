// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UBehaviorTree;
class AThrowables;

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	EET_WalkAround UMETA(DisplayName = "Walk Around"),
	EET_StandStill UMETA(DisplayName = "Stand Still")
};

UCLASS()
class PROJECTCATCH_API AEnemy : public ACharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AEnemy();

	UFUNCTION(BlueprintCallable, Category = Enemy, meta = (DisplayName = "Enemy Sucked"))
	void Sucked(const FVector& SuckLocation);

	UFUNCTION()
	void NoiseHeard(AThrowables* Throwable);

	UFUNCTION(BlueprintCallable, Category = Enemy, meta = (DisplayName = "Set Run State"))
	void SetRunState(const bool bRun) const;

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return Tree;}
	FORCEINLINE EEnemyType GetEnemyType() const { return EnemyType; }
	FORCEINLINE const FTransform& GetOriginalTransform() const { return OriginalTransform; }
	FORCEINLINE TObjectPtr<AThrowables>& GetLastHeardThrowableRef() { return LastHeardThrowable; }
	FORCEINLINE TObjectPtr<AThrowables> GetLastHeardThrowable() const { return LastHeardThrowable; }
	FORCEINLINE bool GetIsHit() const { return bHit; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:
	UPROPERTY(BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true", DisplayName = "Is Hit"))
	bool bHit;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = Enemy, meta = (AllowPrivateAccess = "true", DisplayName = "Enemy Type"))
	EEnemyType EnemyType;

	UPROPERTY(BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true", DisplayName = "Last Heard Throwable"))
	TObjectPtr<AThrowables> LastHeardThrowable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess = "true"))
	UBehaviorTree* Tree;
	
	FTimerHandle SuckHandle;
	float Alpha;
	float DefaultSpeed;
	float RunSpeed;
	FTransform OriginalTransform;
};
