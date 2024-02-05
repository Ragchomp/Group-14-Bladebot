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

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	//Called when player died
	UFUNCTION()
	virtual void PlayerDeath(ACharacter* PlayerCharacter);

	//Signature to bind the delegate
	FOnPlayerDeathSignature OnPlayerDeathSignature;

	UFUNCTION(BlueprintCallable)
	void StartTimer();

	UFUNCTION(BlueprintCallable)
	void StopTimer();

	UFUNCTION(BlueprintCallable)
	void ResetTimer();

	void CountTime();



	/** Time manager*/
public:
	UPROPERTY(BlueprintReadOnly, Category = "Time")
	float Millisecs = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	float Seconds = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	float Minutes = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	bool TimerShouldTick = true;

	FTimerHandle TimerHandeler;

};
