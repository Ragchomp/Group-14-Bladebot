// Fill out your copyright notice in the Description page of Project Settings.


#include "BF_DamageEvents.h"

// Register damageable actor
void UBF_DamageEvents::RegisterDamageableUnit(AActor* DamageableUnit)
{
	// Add to array
	DamageableUnits.Add(DamageableUnit);
}