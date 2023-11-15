// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerCameraComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PlayerCharacter.h"
#include "GrapplingHook/GrapplingHookHead.h"

UPlayerCameraComponent::UPlayerCameraComponent()
{
	////create the speed lines component
	//SpeedLinesRef = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpeedLinesRef"));

	////attach the speed lines to this component
	//SpeedLinesRef->SetupAttachment(this);
}

void UPlayerCameraComponent::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//get owner as a player character
	PlayerCharacterRef = Cast<APlayerCharacter>(GetOwner());

	////check if we shouldn't use the speed lines
	//if (!bUseSpeedLines)
	//{
	//	//check if we have a reference to the speed lines
	//	if (SpeedLinesRef)
	//	{
	//		//destroy the speed lines
	//		SpeedLinesRef->DestroyComponent();
	//	}
	//}
}

void UPlayerCameraComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//check if we have a valid world
	if (!GetWorld()->IsValidLowLevelFast())
	{
		//call the parent implementation
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		//return
		return;
	}

	//check that we're not in a preview world
	if (GetWorld()->IsPreviewWorld())
	{
		//call the parent implementation
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		//return
		return;	
	}

	//check if we have a valid player character reference
	if (PlayerCharacterRef)
	{
		//update the camera state
		UpdateCameraState(PlayerCharacterRef->GrapplingHookRef, DeltaTime);	
	}

	////get the owner's velocity
	//const FVector Velocity = GetOwner()->GetVelocity();

	////get the speed
	//float Speed = Velocity.Size();

	////check if we are ignoring the z axis
	//if (bIgnoreZVel)
	//{
	//	//zero out the z axis
	//	Speed = Velocity.Size2D();
	//}

	////check if we have speed lines and if we should use them
	//if (SpeedLinesRef && bUseSpeedLines)
	//{
	//	//clamp the speed
	//	Speed = FMath::GetMappedRangeValueClamped(SpeedLinesSpeedParamInRange, SpeedLinesSpeedParamOutRange, Speed);

	//	//if the speed is greater than the speed lines velocity
	//	if (bUseDebugVal)
	//	{
	//		//update the float parameter
	//		SpeedLinesRef->SetFloatParameter(SpeedLinesSpeedParamName, SpeedLinesDebugVal);
	//	}
	//	else if (Speed > MinSpeedLinesVal)
	//	{
	//		//update the float parameter
	//		SpeedLinesRef->SetFloatParameter(SpeedLinesSpeedParamName, Speed);
	//	}
	//	else
	//	{
	//		//update the float parameter
	//		SpeedLinesRef->SetFloatParameter(SpeedLinesSpeedParamName, 0.0f);
	//	}
	//}

	//call the parent implementation
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerCameraComponent::UpdateCameraState(AGrapplingHookHead* GrapplingHook, const float DeltaTime)
{
	//the new camera state
	EGrappleState NewState;

	//check if we have a valid grappling hook
	if (GrapplingHook)
	{
		//set the new state
		NewState = GrapplingHook->GrappleState;
	}
	else
	{
		//set the new state
		NewState = EGrappleState::EGS_Retracted;
	}

	//loop through the camera states
	for (const FCameraStateStruct State : CameraStates)
	{
		//check if the state is the same as the new state and if we should use it with whatever dashing state we're in
		if (State.GrappleState == NewState && State.bUseWithDashing == PlayerCharacterRef->GetIsDashing())
		{
			//set the camera state
			CurrentCameraState = State;

			//exit the loop
			break;
		}
	}

	//update the field of view
	FieldOfView = InterpToTarget(CurrentCameraState.FovInterpType, FieldOfView, CurrentCameraState.FieldOfView, CurrentCameraState.FovInterpSpeed, DeltaTime);

	//update the post process settings
	PostProcessSettings = CurrentCameraState.PostProcessSettings;
}
