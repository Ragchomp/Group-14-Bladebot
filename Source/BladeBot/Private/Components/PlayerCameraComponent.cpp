// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerCameraComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GrapplingHook/GrapplingHookHead.h"

UPlayerCameraComponent::UPlayerCameraComponent()
{
}

void UPlayerCameraComponent::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//spawn the speed lines
	//SpeedLinesRef = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpeedLines, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
}

void UPlayerCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	////get the owner's velocity
	//const FVector Velocity = GetOwner()->GetVelocity();

	////get the speed
	//float Speed = Velocity.Size();

	////if we are ignoring the z axis
	//if (bIgnoreZVelocity)
	//{
	//	//zero out the z axis
	//	Speed = Velocity.Size2D();
	//}

	////clamp the speed
	//Speed = FMath::GetMappedRangeValueClamped(SpeedLinesSpeedParamInRange, SpeedLinesSpeedParamOutRange, Speed);

	////if the speed is greater than the speed lines velocity
	//if (Speed > MinSpeedLinesVal)
	//{
	//	//update the float parameter
	//	SpeedLinesRef->SetFloatParameter(SpeedLinesSpeedParamName, Speed);
	//}
	//else
	//{
	//	//update the float parameter
	//	SpeedLinesRef->SetFloatParameter(SpeedLinesSpeedParamName, 0.0f);
	//}

	////if we are using a seperate fov when grappling hook is active
	//if (bUseGrapplingFov)
	//{
	//	switch (GrapplingHookRef->GetGrappleState()) {
	//		case EGrappleState::EGS_Retracted:
	//			//set the fov
	//			CustomSetFieldOfView(HookRetractedFov, DeltaTime, ToRetractedLerpSpeed);
	//			break;
	//		case EGrappleState::EGS_InAir:
	//			//set the fov
	//			CustomSetFieldOfView(HookInAirFov, DeltaTime, ToAirLerpSpeed);
	//			break;
	//		case EGrappleState::EGS_Attached:
	//			//set the fov
	//			CustomSetFieldOfView(HookRetractingFov, DeltaTime, ToRetractingLerpSpeed);
	//			break;
	//		default: ;
	//	}
	//}

	//call the parent implementation
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerCameraComponent::CustomSetFieldOfView(const float InFieldOfView, const float DeltaTime, const float LerpSpeed)
{
	//if we are lerping the fov
	if (bLerpFov)
	{
		//lerp the fov
		SetFieldOfView(InterpToTarget(FovInterpType,FieldOfView, InFieldOfView, DeltaTime, LerpSpeed));
	}
	else
	{
		//set the fov
		SetFieldOfView(InFieldOfView);
	}
}
