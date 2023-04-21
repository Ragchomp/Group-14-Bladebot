// Fill out your copyright notice in the Description page of Project Settings.


#include "BF_AimLimbAtTarget.h"

void UBF_AimLimbAtTarget::AimLimbAtTarget(FVector TargetLocation, FVector TargetVelocity, FVector LimbLocation, FVector LimbVelocity, float LimbSpeed, FVector& OutAimLocation, FVector& OutAimVelocity)
{
	// Calculate the time it takes for the limb to reach the target
	float TimeToTarget = FVector::Dist(TargetLocation, LimbLocation) / LimbSpeed;

	// Calculate the target location at the time the limb reaches the target
	FVector TargetLocationAtTime = TargetLocation + TargetVelocity * TimeToTarget;

	// Calculate the aim location
	OutAimLocation = TargetLocationAtTime;

	// Calculate the aim velocity
	OutAimVelocity = TargetVelocity;
}

void UBF_AimLimbAtTarget::RotateLimb(FRotator CurrentRotation, FRotator TargetRotation, float RotationSpeed, float DeltaTime, FRotator& OutRotation)
{
	// Calculate the rotation delta
	FRotator RotationDelta = TargetRotation - CurrentRotation;

	// Calculate the rotation speed
	float RotationSpeedPerSecond = RotationSpeed * 360.0f;

	// Calculate the rotation delta per second
	FRotator RotationDeltaPerSecond = RotationDelta * RotationSpeedPerSecond;

	// Calculate the rotation delta per frame
	FRotator RotationDeltaPerFrame = RotationDeltaPerSecond * DeltaTime;

	// Calculate the new rotation
	OutRotation = CurrentRotation + RotationDeltaPerFrame;
}