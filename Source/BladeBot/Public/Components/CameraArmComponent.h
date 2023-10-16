// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
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

	//Custom Camera Lag variables

	//the maximum distance the camera can lag behind the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings, meta = (ClampMin = 0.0f, displayname = "Camera Lag Max Distance"))
	float CustomCameraLagMaxDistance = 800;

	//the minimum distance the camera can lag behind the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	float CameraLagMinDistance = 400;

	//the velocity at which the camera will be at its maximum distance from the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	float CameraLagMaxVelocity = 1200;

	//whether to use the character's walking speed as the minimum velocity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	bool bUseCharacterWalkingSpeed = true;

	//the minimum velocity the player must be moving at for the camera to start to lag behind them
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings, meta = (ClampMin = 0.0f, editcondition = "bUseCharacterWalkingSpeed == false", editconditionHides))
	float CameraLagMinVelocity = 600;

	//the maximum amount that the target arm length can be changed by per second by the camera lag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings, DisplayName="Camera Lag Speed", meta=(ClampMin = 0.0f))
	float CustomCameraLagSpeed = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	bool bIgnoreZVelocity = true;

	//the previous alpha value used for the camera lag
	float OldLerpAlpha = 0;

	//reference to the character owner of this component
	UPROPERTY()
	ACharacter* CharacterOwner = nullptr;

	//overrides
	virtual void BeginPlay() override;
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
};
