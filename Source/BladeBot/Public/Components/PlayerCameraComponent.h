// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Camera/CameraComponent.h"
#include "Math/InterpShorthand.h"
#include "PlayerCameraComponent.generated.h"

class AGrapplingHookHead;
/**
 * 
 */
UCLASS()
class BLADEBOT_API UPlayerCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

	public:

	//constructor
	UPlayerCameraComponent();

	//whether or not to use a seperate fov when grappling hook is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
	bool bUseGrapplingFov = true;

	//whether or not to use a seperate fov when grappling hook is retracting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
	bool bUseRetractingFov = true;

	//the fov to use when grappling hook is active
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV", meta = (EditCondition = "bUseGrapplingFov", editconditionHides))
	float HookInAirFov = 90.0f;

	//the fov to use when grappling hook is retracting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV", meta = (EditCondition = "bUseRetractingFov", editconditionHides))
	float HookRetractingFov = 90.0f;

	//the fov to use when grappling hook is retracted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
	float HookRetractedFov = 90.0f;

	//whether to lerp or set the fov
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
	bool bLerpFov = true;

	//the speed at which to lerp the fov to the hook in air fov
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV", meta = (EditCondition = "bLerpFov", editconditionHides))
	float ToAirLerpSpeed = 10.0f;

	//the speed at which to lerp the fov to the hook retracting fov
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV", meta = (EditCondition = "bLerpFov", editconditionHides))
	float ToRetractingLerpSpeed = 10.0f;

	//the speed at which to lerp the fov to the hook retracted fov
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV", meta = (EditCondition = "bLerpFov", editconditionHides))
	float ToRetractedLerpSpeed = 10.0f;

	//the interpolation method to use when lerping the fov
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV", meta = (EditCondition = "bLerpFov", editconditionHides))
	TEnumAsByte<EInterpToTargetType> FovInterpType = InterpTo;

	//the velocity at which the camera speed lines will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	float MinSpeedLinesVal = 30.f;

	//whether or not to ignore the z axis when calculating the speed for the speed lines
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	bool bIgnoreZVelocity = true;

	//the speed lines to use when the camera owner is moving fast
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	UNiagaraSystem* SpeedLines = nullptr;

	//the parameter name for the speed lines's speed parameter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	FName SpeedLinesSpeedParamName = "Speed";

	//the in range for the speed lines's speed parameter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	FVector2D SpeedLinesSpeedParamInRange = FVector2D(0.0f, 90.0f);

	//the out range for the speed lines's speed parameter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	FVector2D SpeedLinesSpeedParamOutRange = FVector2D(0.0f, 6000.0f);

	//reference to the spawned speed lines
	UPROPERTY(BlueprintReadOnly, Category = "Camera|SpeedLines")
	UNiagaraComponent* SpeedLinesRef = nullptr;

	//reference to the grappling hook
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	AGrapplingHookHead* GrapplingHookRef = nullptr;

	//overrides
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void CustomSetFieldOfView(float InFieldOfView, float DeltaTime, float LerpSpeed);
};
