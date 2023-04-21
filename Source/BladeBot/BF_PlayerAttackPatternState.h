// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BF_PlayerAttackPatternState.generated.h"

/**
 * 
 */
UCLASS()
class BLADEBOT_API UBF_PlayerAttackPatternState : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	uint8 AttackPatternIndex = 0;
	
	UFUNCTION(BlueprintCallable, Category = "Attack Pattern")
		void SetAttackPatternState(uint8 NewAttackPatternState);

	UFUNCTION(BlueprintCallable, Category = "Attack Pattern")
		uint8 GetAttackPatternState();

	UFUNCTION(BlueprintCallable, Category = "Attack Pattern")
		void IncrementAttackPatternState();

	UFUNCTION(BlueprintCallable, Category = "Attack Pattern")
		void ResetAttackPatternState();
};
