// Fill out your copyright notice in the Description page of Project Settings.


#include "BladebotGameMode.h"

#include "GameFramework/Character.h"

ABladebotGameMode::ABladebotGameMode()
{
	PrimaryActorTick.bCanEverTick = true;


	//Set default pawn class to our blueprint character
	TSubclassOf<ACharacter> PlayerCharacterBlueprint;
}

void ABladebotGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

void ABladebotGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, FString::Printf(TEXT("Millies: %f"),Millisecs));
	GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("Secs: %f"), Seconds));
	GEngine->AddOnScreenDebugMessage(3, 1, FColor::Green, FString::Printf(TEXT("Mins: %f"), Minutes));
}

void ABladebotGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Binding the PlayerDeath delegate to the GameMode's PlayerDeath() Function
	if (!OnPlayerDeathSignature.IsBound())
	{
		OnPlayerDeathSignature.AddDynamic(this, &ABladebotGameMode::PlayerDeath);
	}

	TimerInit();
}

void ABladebotGameMode::PlayerDeath(ACharacter* PlayerCharacter)
{
	//Getting the reference to the PlayerController
	const TObjectPtr<AController> PlayerController = PlayerCharacter->GetController();
	RestartPlayer(PlayerController);
}

void ABladebotGameMode::TimerInit()
{
	// Each second it increases seconds float by one forever.
	GetWorldTimerManager().SetTimer(TimerHandeler, this, &ABladebotGameMode::CountTime, 1.0f / 1000, true);
}

void ABladebotGameMode::CountTime()
{
	//if the timer shouldn't tick return
	if (TimerShouldTick == false)
		return;

	//increment the seconds
	Millisecs++;

	if (Millisecs >= 1000)
	{
		Seconds++;
		Millisecs = 0;
	}

	//convert seconds to minutes
	if (Seconds >= 60)
	{
		Minutes++;
		Seconds = 0;
	}
}

