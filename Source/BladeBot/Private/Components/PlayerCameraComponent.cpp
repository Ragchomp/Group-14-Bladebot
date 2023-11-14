// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerCameraComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PlayerCharacter.h"
#include "GrapplingHook/GrapplingHookHead.h"

UPlayerCameraComponent::UPlayerCameraComponent()
{
	SpeedLinesRef = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpeedLinesRef"));
}

void UPlayerCameraComponent::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//get owner as a player character
	PlayerCharacterRef = Cast<APlayerCharacter>(GetOwner());

	//check if we shouldn't use the speed lines
	if (!bUseSpeedLines)
	{
		//check if we have a reference to the speed lines
		if (SpeedLinesRef)
		{
			//destroy the speed lines
			SpeedLinesRef->DestroyComponent();
		}
	}
}

void UPlayerCameraComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//check that we're not in the editor
	if (GetWorld()->IsPreviewWorld())
	{
		//call the parent implementation
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		//return
		return;
	}

	//get the owner's velocity
	const FVector Velocity = GetOwner()->GetVelocity();

	//get the speed
	float Speed = Velocity.Size();

	//check if we are ignoring the z axis
	if (bIgnoreZVel)
	{
		//zero out the z axis
		Speed = Velocity.Size2D();
	}

	//check if we have speed lines and if we should use them
	if (SpeedLinesRef && bUseSpeedLines)
	{

		//clamp the speed
		Speed = FMath::GetMappedRangeValueClamped(SpeedLinesSpeedParamInRange, SpeedLinesSpeedParamOutRange, Speed);

		//if the speed is greater than the speed lines velocity
		if (bUseDebugVal)
		{
			//update the float parameter
			SpeedLinesRef->SetFloatParameter(SpeedLinesSpeedParamName, SpeedLinesDebugVal);
		}
		else if (Speed > MinSpeedLinesVal)
		{
			//update the float parameter
			SpeedLinesRef->SetFloatParameter(SpeedLinesSpeedParamName, Speed);
		}
		else
		{
			//update the float parameter
			SpeedLinesRef->SetFloatParameter(SpeedLinesSpeedParamName, 0.0f);
		}
	}


	//check if we have a references
	if (GrapplingHookRef && PlayerCharacterRef)
	{
		//update the camera state
		UpdateCameraState(GrapplingHookRef->GetGrappleState(), PlayerCharacterRef->GetIsDashing(), DeltaTime);
	}

	//call the parent implementation
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerCameraComponent::UpdateCameraState(const EGrappleState NewState, const bool IsDashing, const float DeltaTime)
{
	//init the camera state variable
	FCameraStateStruct CameraState;

	//loop through the camera states
	for (const FCameraStateStruct State : CameraStates)
	{
		//check if the state is the same as the new state and if we should use it with whatever dashing state we're in
		if (State.GrappleState == NewState && State.bUseWithDashing == IsDashing)
		{
			//set the camera state
			CameraState = State;

			//exit the loop
			break;
		
		}
	}

	//update the field of view
	InterpToTarget(CameraState.FovInterpType, FieldOfView, CameraState.FieldOfView, CameraState.FovInterpSpeed, DeltaTime);

	//update the post process settings
	PostProcessSettings = CameraState.PostProcessSettings;
}