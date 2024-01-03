// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectCatchGameMode.h"
#include "UObject/ConstructorHelpers.h"

AProjectCatchGameMode::AProjectCatchGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Core/Player/BP_MyPlayer"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
