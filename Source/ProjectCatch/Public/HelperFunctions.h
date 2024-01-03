// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HelperFunctions.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCATCH_API UHelperFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	FORCEINLINE static void ConstructEditorRotator(FRotator& Rotator, const float Roll, const float Pitch, const float Yaw) {Rotator = FRotator(Pitch, Yaw, Roll);}
	FORCEINLINE static void QuickPrint(const FString& Text) {GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Text);}
	static bool SimpleSingleLineTrace(const UObject* Self, FHitResult& Result, const FVector& StartLocation, const FVector& EndLocation, const ECollisionChannel TraceChannel,
		const bool bDisplayDebugLine = false, const FColor& Color = FColor::Red, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
		const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);
};
