// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Enemy/EQS/EnvQueryContext_Player.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	const auto World = GetWorld();
	if(!IsValid(World)) return;

	if(const auto Player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(World, 0)))
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, Player);
		return;
	}

	if(const auto Start = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass()))
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, Start);
	}
}
