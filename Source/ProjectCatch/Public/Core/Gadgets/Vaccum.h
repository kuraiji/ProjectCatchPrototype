// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vaccum.generated.h"

class UCableComponent;
class AEnemy;

UCLASS()
class PROJECTCATCH_API AVaccum : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVaccum();
	
	UStaticMeshComponent* GetBackpack() {return Backpack;}
	USceneComponent* GetNozzleGrip() {return NozzleGrip;}
	
	void Sucking();
	void StopSucking();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Vaccum, meta=(DisplayName = "On Enemy Suck"))
	void OnEnemySuck(const AEnemy* SuckedEnemy);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Outer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Backpack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Nozzle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> NozzleGrip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> NozzleHole;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cable, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCableComponent> Cable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> NozzleFront;

	UPROPERTY(EditDefaultsOnly, Category = Vaccum, meta = (AllowPrivateAccess = "true", DisplayName = "Line Trace Scaler"))
	float LineTraceScaler;

	UPROPERTY(EditDefaultsOnly, Category = Vaccum, meta = (AllowPrivateAccess = "true", DisplayName = "Sucking Delay"))
	float SuckingDelay;
	
	UPROPERTY(EditDefaultsOnly, Category = Vaccum, meta = (AllowPrivateAccess = "true", DisplayName = "Sucking Delay"))
	float BoxHalfSize;

	UPROPERTY()
	TObjectPtr<AEnemy> SuckingActor;

	FTimerHandle SuckHandle;
};
