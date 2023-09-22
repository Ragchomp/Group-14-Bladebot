// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BladebotGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDeathSignature, ACharacter*, PlayerCharacter);

/**
 * 
 */
UCLASS()
class BLADEBOT_API ABladebotGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABladebotGameMode();

	const FOnPlayerDeathSignature& GetOnPlayerDeath() const { return OnPlayerDeathSignature; }

	//Spawning the player
	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;

	//Called when player died
	virtual void PlayerDeath(ACharacter* PlayerCharacter);

	//Signature to bind the delegate
	FOnPlayerDeathSignature OnPlayerDeathSignature;

};
