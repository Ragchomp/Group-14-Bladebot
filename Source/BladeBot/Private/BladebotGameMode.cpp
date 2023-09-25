// Fill out your copyright notice in the Description page of Project Settings.


#include "BladebotGameMode.h"

#include "GameFramework/Character.h"

ABladebotGameMode::ABladebotGameMode()
{
	//Set default pawn class to our blueprint character
	TSubclassOf<ACharacter> PlayerCharacterBlueprint;
}

void ABladebotGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

void ABladebotGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Binding the PlayerDeath delegate to the GameMode's PlayerDeath() Function
	if (!OnPlayerDeathSignature.IsBound())
	{
		OnPlayerDeathSignature.AddDynamic(this, &ABladebotGameMode::PlayerDeath);
	}
}

void ABladebotGameMode::PlayerDeath(ACharacter* PlayerCharacter)
{
	//Getting the reference to the PlayerController
	const TObjectPtr<AController> PlayerController = PlayerCharacter->GetController();
	RestartPlayer(PlayerController);
}
