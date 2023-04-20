// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_DamageEventContainer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UI_DamageEventContainer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLADEBOT_API II_DamageEventContainer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// OnPlayerBounce
	// When the player hits an enemy at speed
	// Returns whether the enemy was killed
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage Events")
	bool OnPlayerBounce(AActor* Player, float Speed);

	// OnPlayerAttack
	// When the player hits an enemy with an attack
	// Returns whether the enemy was killed
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage Events")
	bool OnPlayerAttack(AActor* Player, float Speed);

	// OnPlayerStab
	// When the player hits an enemy at speed with a stab
	// Returns whether the enemy was killed
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage Events")
	bool OnPlayerStab(AActor* Player, float Speed);

};
