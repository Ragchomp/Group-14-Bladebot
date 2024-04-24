// Fill out your copyright notice in the Description page of Project Settings.


#include "BladebotGameMode.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ABladebotGameMode::ABladebotGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABladebotGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

void ABladebotGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, FString::Printf(TEXT("Millies: %f"),Millisecs));
	//GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("Secs: %f"), Seconds));
	//GEngine->AddOnScreenDebugMessage(3, 1, FColor::Green, FString::Printf(TEXT("Mins: %f"), Minutes));
}

void ABladebotGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Binding the PlayerDeath delegate to the GameMode's PlayerDeath() Function
	if (!OnPlayerDeathSignature.IsBound())
	{
		OnPlayerDeathSignature.AddDynamic(this, &ABladebotGameMode::PlayerDeath);
	}

	//StartTimer();
}

void ABladebotGameMode::PlayerDeath(ACharacter* PlayerCharacter)
{
	//Getting the reference to the PlayerController
	const TObjectPtr<AController> PlayerController = PlayerCharacter->GetController();
	RestartPlayer(PlayerController);
}

void ABladebotGameMode::StartTimer()
{
	// Each second it increases seconds float by one forever.
	TimerShouldTick = true;
	GetWorldTimerManager().SetTimer(TimerHandeler, this, &ABladebotGameMode::CountTime, 1.0f / 1000, true);

	//set the last time the timer was started
	LastTimerStart = GetWorld()->TimeSeconds;
}

void ABladebotGameMode::StopTimer()
{
	GetWorldTimerManager().ClearTimer(TimerHandeler);
	TimerShouldTick = false;
}

void ABladebotGameMode::CountTime()
{
	//if the timer shouldn't tick return
	if (TimerShouldTick == false)
		return;
	
	//Get the current time of the world
	const float PlaySeconds = GetWorld()->TimeSeconds - LastTimerStart;

	//Calculate the milliseconds
	Millisecs = FMath::Floor(FMath::Fmod(PlaySeconds, 1) * 1000);

	//Calculate the seconds
	Seconds = int(FMath::Floor(PlaySeconds)) % 60;

	//Calculate the minutes
	Minutes = FMath::Floor(PlaySeconds / 60);
}

void ABladebotGameMode::ResetTimer()
{
	Millisecs = 0;
	Seconds = 0;
	Minutes = 0;
}

