// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Camera/CameraComponent.h"
#include "Characters/StateControl.h"
#include "Math/InterpShorthand.h"
#include "PlayerCameraComponent.generated.h"

class APlayerCharacter;
class AGrapplingHookHead;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FCameraStateStruct
{
	GENERATED_BODY()

	//constructors
	explicit FCameraStateStruct(const EGrappleState InGrappleState = EGrappleState::EGS_Retracted, const float InFov = 90.0f, const bool InUseWithDashing =  false, const FPostProcessSettings InPostProcessSettings = FPostProcessSettings())
	{
		GrappleState = InGrappleState;
		PostProcessSettings = InPostProcessSettings;
		FieldOfView = InFov;
		bUseWithDashing = InUseWithDashing;
	}

	//the grapple state to use this camera state for
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGrappleState GrappleState = EGrappleState::EGS_Retracted;

	//whether or not this state is meant to be used when dashing
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseWithDashing = false;

	//the post process settings to use
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPostProcessSettings PostProcessSettings = FPostProcessSettings();

	//the Fov to use
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FieldOfView = 90.0f;

	//the interpolation method to use when lerping the fov
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EInterpToTargetType> FovInterpType = InterpTo;

	//the speed at which to lerp the fov
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FovInterpSpeed = 10.0f;
};

UCLASS()
class BLADEBOT_API UPlayerCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

	public:

	//constructor
	UPlayerCameraComponent();

	//array of camera states
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TArray<FCameraStateStruct> CameraStates = {
		FCameraStateStruct(EGrappleState::EGS_Retracted),
		FCameraStateStruct(EGrappleState::EGS_InAir),
		FCameraStateStruct(EGrappleState::EGS_Attached),
	};

	//the velocity at which the camera speed lines will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	float MinSpeedLinesVal = 30.f;

	//whether or not to ignore the z axis when calculating the speed for the speed lines
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	bool bIgnoreZVel = true;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|SpeedLines")
	UNiagaraComponent* SpeedLinesRef = nullptr;

	//whether of not to use the speed lines
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	bool bUseSpeedLines = false;

	//debug value for the speed lines
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	float SpeedLinesDebugVal = 1000.0f;

	//whether or not to use the debug value for the speed lines
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SpeedLines")
	bool bUseDebugVal = true;

	//reference to the grappling hook
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	AGrapplingHookHead* GrapplingHookRef = nullptr;

	//reference to the owner as a player character
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	APlayerCharacter* PlayerCharacterRef = nullptr;

	//overrides
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	//updates the camera state
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void UpdateCameraState(const EGrappleState NewState, const bool IsDashing, const float DeltaTime);
};