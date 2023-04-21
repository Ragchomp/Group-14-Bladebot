// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BF_DamageEvents.generated.h"

/**
 * 
 */
UCLASS()
class BLADEBOT_API UBF_DamageEvents : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		
		TArray<AActor*> DamageableUnits;
		
		// Register a damageable actor
		UFUNCTION(BlueprintCallable, Category = "Damage Events")
			void RegisterDamageableUnit(AActor* DamageableUnit);

		// Get all damageable actors
		UFUNCTION(BlueprintCallable, Category = "Damage Events")
			TArray<AActor*> GetDamageableUnits() { return DamageableUnits; }

		// Get number of damageable actors
		UFUNCTION(BlueprintCallable, Category = "Damage Events")
			int GetNumDamageableUnits() { return DamageableUnits.Num(); }
	
};
