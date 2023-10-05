// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/InterpShorthand.h"
#include "CameraArmComponent.generated.h"

class ACharacter;

/**
 * 
 */
UCLASS()
class BLADEBOT_API UCameraArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:

	//constructor
	explicit UCameraArmComponent(FObjectInitializer const& ObjectInitializer);

	//the type of interpolation to use for the camera lag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings|Interpolation")
	TEnumAsByte<EInterpType> InterpolationType = Linear;

	//the control for the interpolation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings|Interpolation", meta=(ClampMin = 0.0f, editcondition = "InterpolationType != Linear", editconditionHides))
	float InterpolationControl = 2;

	//whether to use the character's walking speed as the minimum velocity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings|OwnerSpeed", displayname = "Use Character Walking Speed As Camera Owner Min Speed")
	bool bUseCharWalkSpeed = true;

	//the minimum velocity the player must be moving at for the camera to start to lag behind them
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings|OwnerSpeed", meta = (ClampMin = 0.0f, editcondition = "bUseCharWalkSpeed == false", editconditionHides))
	float CameraOwnerMinSpeed = 600;

	//the velocity at which the camera will be at its maximum distance from the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings|OwnerSpeed", meta=(ClampMin = 0.0f))
	float CameraOwnerMaxSpeed = 1200;

	//whether to ignore the z velocity of our owner when calculating the camera lag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings|OwnerSpeed")
	bool bIgnoreZVelocity = true;

	//the minimum arm length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings|ArmLength", meta=(ClampMin = 0.0f))
	float MinArmLength = 400;

	//the maximum arm length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings|ArmLength", meta=(ClampMin = 0.0f))
	float MaxArmLength = 800;

	//the maximum amount that the target arm length can be changed by per second by the camera lag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings|ArmLength", meta=(ClampMin = 0.0f))
	float MaxLagSpeed = 100;

	//reference to the character owner of this component
	UPROPERTY()
	ACharacter* CharacterOwner = nullptr;

	//the TargetArmLength from the last time CameraLag was applied
	float OldTargetArmLength;

	float test;
	float test2;

	//overrides
	virtual void BeginPlay() override;
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
};
