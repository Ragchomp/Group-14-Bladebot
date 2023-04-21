// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BF_AimLimbAtTarget.generated.h"

/**
 * 
 */
UCLASS()
class BLADEBOT_API UBF_AimLimbAtTarget : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Limb Targeting")
		static void AimLimbAtTarget(FVector TargetLocation, FVector TargetVelocity, FVector LimbLocation, FVector LimbVelocity, float LimbSpeed, FVector& OutAimLocation, FVector& OutAimVelocity);
		
	// Rotate a given limb
	UFUNCTION(BlueprintCallable, Category = "Limb Targeting")
		static void RotateLimb(FRotator CurrentRotation, FRotator TargetRotation, float RotationSpeed, float DeltaTime, FRotator& OutRotation);

	
};
