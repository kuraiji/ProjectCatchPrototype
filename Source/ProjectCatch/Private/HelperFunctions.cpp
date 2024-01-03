// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctions.h"

bool UHelperFunctions::SimpleSingleLineTrace(const UObject* Self, FHitResult& Result, const FVector& StartLocation, const FVector& EndLocation, const ECollisionChannel TraceChannel,
	const bool bDisplayDebugLine, const FColor& Color, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam)
{
	const UWorld* World = Self->GetWorld();
	if(World == nullptr) return false;
	if(bDisplayDebugLine)
	{
		DrawDebugLine(World, StartLocation,EndLocation, Color, false, 1, 0, 1);
	}
	return World->LineTraceSingleByChannel(Result, StartLocation, EndLocation, TraceChannel, Params, ResponseParam);
}