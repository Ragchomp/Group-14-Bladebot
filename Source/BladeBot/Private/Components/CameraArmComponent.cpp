#include "Components/CameraArmComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UCameraArmComponent::UCameraArmComponent(const FObjectInitializer& ObjectInitializer)
{
	//set the default values
	CameraLagMaxDistance = 800;
}

void UCameraArmComponent::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//check if the owner is a character
	if (GetOwner()->IsA(ACharacter::StaticClass()))
	{
		//cast the owner to a character
		CharacterOwner = Cast<ACharacter>(GetOwner());
	}
	else
	{
		//print an error message
		UE_LOG(LogTemp, Error, TEXT("The owner of the CameraArmComponent is not a character!"));
	}

	//set default target offset
	DefaultTargetOffset = TargetOffset;

	//check if we smooth cameramovement when crouching
	if (bSmoothCrouch)
	{
		//set the timer handle
		GetWorld()->GetTimerManager().SetTimer(CrouchLerpTimerHandle, this, &UCameraArmComponent::LerpCameraOffset, CrouchLerpTime, true);
	}

	//check if we use camera zoom
	if (bUseCameraZoom)
	{
		//check if camera lerps is empty
		if (CamZoomInterps.IsEmpty())
		{
			//add a default value to the map
			CamZoomInterps.Add(FCameraZoomStruct());

			//print a warning
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("The LerpArray in the CameraArmComponent is empty!"));
		}

		//set the timer handle
		GetWorld()->GetTimerManager().SetTimer(ZoomInterpTimerHandle, this, &UCameraArmComponent::InterpCameraZoom, CameraZoomUpdateSpeed, true);
	}
}

void UCameraArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	//check if we should apply camera offset and if the character owner is valid
	if (bApplyCameraOffset && CharacterOwner)
	{
		//get the controller rotation
		FRotator ControllerRotation = CharacterOwner->GetControlRotation();

		//remove the pitch from the controller rotation
		ControllerRotation.Pitch = 0;

		//get the camera offset
		const FVector CamSocketOffset = UKismetMathLibrary::GetRightVector(ControllerRotation);

		//set the socket offset
		SocketOffset = CamSocketOffset * CameraOffsetAmount;
	}

	//check if we should only trace the floor
	if (bTraceOnlyFloor)
	{
		//call the parent implementation without tracing
		Super::UpdateDesiredArmLocation(false, bDoLocationLag, bDoRotationLag, DeltaTime);

		//the collision parameters to use
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetOwner());

		//trace from the previous arm origin to the desired location
		FHitResult Result;
		GetWorld()->SweepSingleByChannel(Result, PreviousArmOrigin, PreviousDesiredLoc, FQuat::Identity, ProbeChannel, FCollisionShape::MakeSphere(ProbeSize), QueryParams);

		//check if we hit something
		if (Result.bBlockingHit)
		{
			//check if the hit normal is facing up
			if (Result.ImpactNormal.Z > 0)
			{
				//set the relative socket location
				RelativeSocketLocation = BlendLocations(UnfixedCameraPosition, Result.Location, Result.bBlockingHit, DeltaTime);
			}
		}
	}
	else
	{
		//call the parent implementation without any changes
		Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);		
	}
}

void UCameraArmComponent::InterpCameraZoom()
{
	//the speed of movement
	float Speed;

	//check if we should ignore the z axis
	if (bIgnoreZVelocity)
	{
		//set the speed in 2d
		Speed = GetOwner()->GetVelocity().Size2D();
	}
	else
	{
		//set the speed regularly
		Speed = GetOwner()->GetVelocity().Length();
	}

	//check if current interp index is above 0
	if (CurrentInterpIndex > 0)
	{
		//check if the speed is below the current interp's speed threshold
		if (Speed < CamZoomInterps[CurrentInterpIndex].SpeedThreshold)
		{
			//decrease the current interp index
			CurrentInterpIndex--;
		}
	}

	//check if current interp index is below the max index
	if (CurrentInterpIndex < CamZoomInterps.Num() - 1)
	{
		//check if the speed is above the current interp's speed threshold
		if (Speed > CamZoomInterps[CurrentInterpIndex].SpeedThreshold)
		{
			//increase the current interp index
			CurrentInterpIndex++;
		}
	}
	//get the world delta time
	const float WorldDeltaTime = GetWorld()->GetDeltaSeconds();

	//get the new target arm length
	const float NewTargetArmLength = InterpToTarget<float>(CamZoomInterps[CurrentInterpIndex].InterpType, TargetArmLength, CamZoomInterps[CurrentInterpIndex].TargetArmLength, WorldDeltaTime, CamZoomInterps[CurrentInterpIndex].InterpSpeed);

	//apply the new target arm length
	TargetArmLength = NewTargetArmLength;
}

void UCameraArmComponent::OnStartCrouch(const float ScaledHalfHeightAdjust)
{
	//set the target offset's z value
	TargetOffset.Z = DefaultTargetOffset.Z + ScaledHalfHeightAdjust;
}

void UCameraArmComponent::OnEndCrouch(const float ScaledHalfHeightAdjust)
{
	//set the target offset's z value
	TargetOffset.Z = DefaultTargetOffset.Z - ScaledHalfHeightAdjust;
}

void UCameraArmComponent::LerpCameraOffset()
{
	//get the world delta time
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	//lerp the target offset
	TargetOffset.Z = UKismetMathLibrary::FInterpTo(TargetOffset.Z, DefaultTargetOffset.Z, DeltaTime, CrouchLerpSpeed);
}
