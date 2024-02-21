#include "Components/PlayerMovementComponent.h"
#include "Components/PlayerCameraComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Objectives/ObjectivePoint.h"

FGrappleInterpStruct::FGrappleInterpStruct(const float InPullSpeed, const float InPullAccel, const EInterpToTargetType InInterpMode): InInterpMode(InInterpMode), PullSpeed(InPullSpeed), PullAccel(InPullAccel)
{

}

UPlayerMovementComponent::UPlayerMovementComponent()
{
	bUseFlatBaseForFloorChecks = true;
	bApplyGravityWhileJumping = false;
	MaxWalkSpeed = 1200.f;
	BrakingFrictionFactor = 0.1f;
	JumpZVelocity = 800.f;
	AirControl = 2.f;
	GravityScale = 4.f;
	FallingLateralFriction = 4.f;
	MaxFlySpeed = 200000.f;
}

void UPlayerMovementComponent::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//array of player camera components
	TArray<UPlayerCameraComponent*> PlayerCameras;

	//get all of the player camera components
	GetOwner()->GetComponents(PlayerCameras);

	//check if there are any player camera components
	if (!PlayerCameras.IsEmpty())
	{
		//set the player camera
		PlayerCamera = PlayerCameras[0];
	}
}

void UPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//call the parent implementation
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//check if the player is grappling
	if (bIsGrappling)
	{
		//set the grapple velocity
		UpdateGrappleVelocity(DeltaTime);
	}

	//do wall running checks and updates
	bIsWallRunning = DoWallRunning(DeltaTime);

	//do wall latch checks and updates
	bIsWallLatching = DoWallLatch(DeltaTime);

	////reset bDidRotateThisFrame
	//bDidRotateThisFrame = false;
}

FVector UPlayerMovementComponent::NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const
{
	//check if we'er wall running
	if (bIsWallRunning)
	{
		//return zero vector
		return InitialVelocity;
	}

	//check if we're wall latching
	if (bIsWallLatching)
	{
		//return zero vector
		return FVector::ZeroVector;
	}

	//store the result of the parent implementation
	FVector Result = Super::NewFallVelocity(InitialVelocity, Gravity, DeltaTime);

	//check if jump is providing force
	if (GetCharacterOwner()->JumpForceTimeRemaining > 0 && bLastJumpWasDirectional)
	{
		//add the directional jump glide force to the result
		Result += LastDirectionalJumpDirection * DirectionalJumpGlideForce * DeltaTime;
	}

	return Result;
}

void UPlayerMovementComponent::Launch(FVector const& LaunchVel)
{
	//call the parent implementation
	Super::Launch(LaunchVel);

	//check if the player is grappling and we should use the flying movement mode
	if (bIsGrappling && bUseFlyingMovementMode)
	{
		//set the movement mode to flying
		SetMovementMode(MOVE_Flying);
	}
}

bool UPlayerMovementComponent::DoJump(bool bReplayingMoves)
{
	//check if we're wall latching
	if (bIsWallLatching)
	{
		//launch off the wall latch
		LaunchOffWallLatch();

		//return true
		return true;
	}

	//check if we're wall running
	if (bIsWallRunning)
	{
		//do a wall run jump
		DoWallRunJump(WallRunningHitResult);

		//return true
		return true;
	}

	//check if we're moving fast enough to do a boosted jump and we're on the ground
	if (Velocity.Length() >= MinSpeedForSpeedBoost && !IsFalling())
	{
		//update bLastJumpWasDirectional
		bLastJumpWasDirectional = true;

		//do a boosted jump
		BoostJump(JumpZVelocity);

		//return true
		return true;
	}

	//try the parent implementation
	if (Super::DoJump(bReplayingMoves))
	{
		//update bLastJumpWasDirectional
		bLastJumpWasDirectional = false;

		//call the blueprint event
		OnNormalJump.Broadcast();

		//return true
		return true;
	}

	//print debug message
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Jumping failed")));

	//return false
	return false;
}

FVector UPlayerMovementComponent::ConsumeInputVector()
{
	//check if we're wall latching
	if (bIsWallLatching)
	{
		//return zero vector
		return FVector::ZeroVector;
	}

	//check if we're in the rotation mode
	if (bRotationMode)
	{
		//check if the player is actually moving
		if (GetCharacterOwner()->GetPendingMovementInputVector().IsNearlyZero())
		{
			//return zero vector
			return FVector::ZeroVector;
		}

		////recalculate the rotation force left
		//RotationForceLeft = RotationForceLeft + (GetWorld()->GetTimeSeconds() - LastRotationInputTime) * RotationSpeed;

		////check if the rotation force left is greater than 0
		//if (RotationForceLeft > 0.f)
		//{
			////check if the rotation force left is greater than the rotation force
			//if (RotationForceLeft > MaxRotationForce)
			//{
			//	//set the rotation force left to the rotation force
			//	RotationForceLeft = MaxRotationForce;
			//}

			////get how long the player has been holding the rotation input
			//const float RotationInputTime = GetWorld()->GetTimeSeconds() - LastRotationInputTime;

			//rotate the character
			GetOwner()->AddActorWorldRotation(FRotator(GetCharacterOwner()->GetPendingMovementInputVector().X, GetCharacterOwner()->GetPendingMovementInputVector().Y, GetCharacterOwner()->GetPendingMovementInputVector().Z), false, nullptr);

			////set the last rotation input time
			//LastRotationInputTime = GetWorld()->GetTimeSeconds();

			//return zero vector
			return FVector::ZeroVector;
		//}
	}

	//Store the input vector
	FVector ReturnVec = Super::ConsumeInputVector();

	//check if we can change grapple mode
	if (bCanChangeGrappleMode)
	{
		//check if the input vector is zero
		if (ReturnVec.IsNearlyZero())
		{
			//set the grapple mode to set velocity
			GrappleMode = InterpVelocity;
		}
		else
		{
			//set the grapple mode to add to velocity
			GrappleMode = AddToVelocity;
		}
	}

	//check if the player is grappling and we have valid angle and distance input curves
	if (bIsGrappling && GrappleMovementAngleInputCurve && GrappleMovementDistanceInputCurve)
	{
		//get the dot product of the current grapple direction and the return vector
		const float DotProduct = FVector::DotProduct(GetOwner()->GetActorUpVector(), ReturnVec.GetSafeNormal());

		//get the grapple angle movement input curve value
		const float GrappleMovementInputAngleCurveValue = GrappleMovementAngleInputCurve->GetFloatValue(DotProduct);

		//get the grapple distance movement input curve value
		const float GrappleMovementInputDistanceCurveValue = GrappleMovementDistanceInputCurve->GetFloatValue(FMath::Clamp(FVector::Dist(GetOwner()->GetActorLocation(), GrappleRope->GetGrapplePoint(GetCharacterOwner())) / MaxGrappleDistance, 0, 1));

		//multiply the return vector by the grapple movement input curve value, the grapple distance movement input curve value, and the grapple movement input modifier
		ReturnVec *= GrappleMovementInputAngleCurveValue * GrappleMovementInputDistanceCurveValue * GrappleMovementInputModifier;
	}

	//return the return vector
	return ReturnVec;
}

bool UPlayerMovementComponent::ShouldRemainVertical() const
{
	//check if we're in the rotation mode
	if (MovementMode != MOVE_Walking && bRotationMode)
	{
		//return false
		return false;
	}
	//check if we're grappling
	if (bIsGrappling)
	{
		//return false
		return false;
	}

	return Super::ShouldRemainVertical();
}

bool UPlayerMovementComponent::IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const
{
	//if this is a valid landing spot, return true
	if (Super::IsValidLandingSpot(CapsuleLocation, Hit))
	{
		return true;
	}

	//check if the distance from the capsule location to the hit is greater than the capsule half height (to prevent the character from getting stuck on the floor)
	if (const float Distance = FVector::Dist(CapsuleLocation, Hit.ImpactPoint); Distance > GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
	{
		return false;
	}

	//check if the surface normal is facing up and the surface is walkable
	if (Hit.ImpactNormal.Z >= GetWalkableFloorZ() && IsWalkable(Hit))
	{
		//return true
		return true;
	}

	return false;
}

float UPlayerMovementComponent::GetGravityZ() const
{
	//check if the player is grappling, wall latching, or wall running
	if (bIsGrappling || bIsWallLatching || bIsWallRunning)
	{
		return 0.f;
	}

	return Super::GetGravityZ();
}

float UPlayerMovementComponent::GetMaxSpeed() const
{
	//check if we're wall latching
	if (bIsWallLatching)
	{
		//return zero
		return 0.f;
	}

	//Check if the player is grappling
	if (bIsGrappling)
	{
		//return the max speed when grappling
		return GrappleMaxSpeed;
	}

	//check if we're falling
	if (IsFalling())
	{
		//return the max fall speed
		return MaxFallSpeed;
	}

	return Super::GetMaxSpeed();
}

float UPlayerMovementComponent::GetMaxAcceleration() const
{
	//check if we're wall latching
	if (bIsWallLatching)
	{
		//return zero
		return 0.f;
	}

	//Check if the player is grappling
	if (bIsGrappling)
	{
		//return the max acceleration when grappling
		return GrappleMaxAcceleration;
	}

	return Super::GetMaxAcceleration();
}

void UPlayerMovementComponent::StartGrapple(AGrapplingRopeActor* InGrappleRope, const FHitResult InGrappleHit)
{
	//check if the player is already grappling
	if (bIsGrappling)
	{
		return;
	}

	//set is grappling to true
	bIsGrappling = true;

	//check if what we've hit is an objective
	if (InGrappleHit.GetActor()->ActorHasTag(ObjectiveTag))
	{
		//set the grapple hit type to objective
		GrappleHitType = Objective;

		//disable the player's ability to change grapple mode
		bCanChangeGrappleMode = false;
	}
	else
	{
		GrappleHitType = Normal;
	}

	//set the grapple object
	GrappleRope = InGrappleRope;

	//check if we should use the flying movement mode
	if (bUseFlyingMovementMode)
	{
		//set the movement mode to flying
		SetMovementMode(MOVE_Flying);
	}

	//call the start grapple event
	OnStartGrapple.Broadcast(InGrappleHit, GrappleHitType);
}

void UPlayerMovementComponent::StopGrapple()
{
	//check if the player is grappling
	if (bIsGrappling)
	{
		//set is grappling to false
		bIsGrappling = false;

		//reset the character's rotation
		GetCharacterOwner()->SetActorRotation(FRotator(0.f, 0, 0.f));

		//check if we should use the flying movement mode
		if (bUseFlyingMovementMode)
		{
			//set the movement mode back to default
			SetMovementMode(MOVE_Falling);
		}

		//check if we should apply a speed boost
		if (bEndGrappleSpeedBoost)
		{
			//check if we should apply the boost in the direction the player is looking
			if (bEndGrappleBoostInLookDirection)
			{
				//set the direction to launch in
				EndGrappleBoostMovementParams.WorldDirection = PlayerCamera->GetForwardVector();
			}
			else
			{
				//add the boost amount to the velocity
				EndGrappleBoostMovementParams.WorldDirection = Velocity.GetSafeNormal();
			}

			//activate the root motion
			UAsyncRootMovement::AsyncRootMovement(this, this, EndGrappleBoostMovementParams)->Activate();
		}
	}
}

bool UPlayerMovementComponent::CanGrapple() const
{
	//do a line trace to see if the player is aiming at something within grapple range
	FHitResult GrappleHit;
	GrappleLineTrace(GrappleHit, MaxGrappleDistance);

	//return whether the line trace hit something or not
	return GrappleHit.bBlockingHit;
}

float UPlayerMovementComponent::GetGrappleDistanceLeft() const
{
	//do a line trace to see if the player is aiming at something within the grapple check range
	FHitResult GrappleHit;
	GrappleLineTrace(GrappleHit, MaxGrappleCheckDistance);

	//if the line trace hit something, return the distance to the hit
	if (GrappleHit.bBlockingHit)
	{
		//get the distance left until the player can grapple to where they are aiming and check if it's greater than 0
		if (const float GrappleDistanceLeft = FVector::Dist(GetOwner()->GetActorLocation(), GrappleHit.ImpactPoint) - MaxGrappleDistance; GrappleDistanceLeft > 0.f)
		{
			//return the distance left until the player can grapple to where they are aiming
			return GrappleDistanceLeft;
		}
	}

	//otherwise return 0
	return 0.f;
}

void UPlayerMovementComponent::GrappleLineTrace(FHitResult& OutHit, const float MaxDistance) const
{
	//get the camera location and rotation
	FVector CameraLocation;
	FRotator CameraRotation;

	//set the camera location and rotation
	GetOwner()->GetNetOwningPlayer()->GetPlayerController(GetWorld())->GetPlayerViewPoint(CameraLocation, CameraRotation);

	//get the forward vector of the camera rotation
	const FVector Rotation = CameraRotation.Quaternion().GetForwardVector();

	//get the end point of the line trace
	const FVector End = CameraLocation + Rotation * MaxDistance;

	//the collision parameters to use for the line trace
	FCollisionQueryParams GrappleCollisionParams;

	//ignore the owner of the grapple hook
	GrappleCollisionParams.AddIgnoredActor(GetOwner());

	//set the collision shape
	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = CanGrappleCollisionShape;

	//do the line trace
	GetWorld()->SweepSingleByChannel(OutHit, CameraLocation, End, FQuat::Identity, CanGrappleTraceChannel, CollisionShape, GrappleCollisionParams);
}

void UPlayerMovementComponent::DoInterpGrapple(const float DeltaTime, FVector& GrappleVelocity, const FGrappleInterpStruct GrappleInterpStruct)
{
	switch (GrappleInterpStruct.InterpMode)
	{
		case Constant:
			//interpolate the velocity
			GrappleVelocity = FMath::VInterpConstantTo(Velocity, GrappleDirection.GetSafeNormal() * GrappleInterpStruct.PullSpeed, DeltaTime, GrappleInterpStruct.PullAccel);
			break;
		case InterpTo:
			//interpolate the velocity
			GrappleVelocity = FMath::VInterpTo(Velocity, GrappleDirection.GetSafeNormal() * GrappleInterpStruct.PullSpeed, DeltaTime, GrappleInterpStruct.PullAccel);
			break;
		case InterpStep:
			//interpolate the velocity
			GrappleVelocity = FMath::VInterpTo(Velocity, GrappleDirection.GetSafeNormal() * GrappleInterpStruct.PullSpeed, DeltaTime, GrappleInterpStruct.PullAccel);
			break;
	}
}

void UPlayerMovementComponent::UpdateGrappleVelocity(const float DeltaTime)
{
	//check if we're on the ground
	if (IsWalking() && !bUseFlyingMovementMode)
	{
		//set the movement mode to falling to prevent the character from getting stuck on the floor
		SetMovementMode(MOVE_Falling);
	}

	//get the point the character is grappling to
	const FVector GrapplePoint = GrappleRope->GetGrapplePoint(GetCharacterOwner());

	//get the vector from the character to the grapple point
	GrappleDirection = (GrapplePoint - GetCharacterOwner()->GetActorLocation()).GetSafeNormal();

	//storage for the velocity that will be applied from the grapple
	FVector GrappleVelocity;

	//check if the hit type is objective
	if (GrappleHitType == Objective)
	{
		//do the interpolation
		DoInterpGrapple(DeltaTime, Velocity, ObjectiveGrappleInterpStruct);
	}
	else
	{
		//check how we should set the velocity
		// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
		switch (GrappleMode)
		{
			case AddToVelocity:
				//add the grapple vector to the character's velocity
				GrappleVelocity = GrappleDirection * WasdPullSpeed * DeltaTime;

			//get the dot product of the character's velocity and the grapple velocity
			GrappleDotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), GrappleVelocity.GetSafeNormal());

			//check if we have a valid grapple velocity curve
			if (GrappleAngleVelocityCurve && GrappleDistanceVelocityCurve)
			{
				//get the grapple angle velocity curve value
				const float GrappleAngleVelocityCurveValue = GrappleAngleVelocityCurve->GetFloatValue(GrappleDotProduct);

				//get the grapple distance velocity curve value
				const float GrappleDistanceVelocityCurveValue = GrappleDistanceVelocityCurve->GetFloatValue(FMath::Clamp(FVector::Dist(GetOwner()->GetActorLocation() , GrapplePoint) / MaxGrappleDistance, 0, 1));

				//multiply the grapple velocity by the grapple velocity curve value
				GrappleVelocity *= GrappleAngleVelocityCurveValue * GrappleDistanceVelocityCurveValue;
			}

			//apply the grapple velocity
			Velocity += GrappleVelocity;

			break;
			case InterpVelocity:
				//do the interpolation
				DoInterpGrapple(DeltaTime, Velocity, NoWasdGrappleInterpStruct);

			break;
		}
	}

	//update the character's velocity
	UpdateComponentVelocity();

	//check if we're not in the rotation mode
	if (!bRotationMode)
	{
		//set the rotation of the character to the grapple direction
		GetOwner()->SetActorRotation(GrappleDirection.Rotation());
	}

}

void UPlayerMovementComponent::BoostJump(const float JumpZVel)
{
	//get the direction of the jump
	LastDirectionalJumpDirection = PlayerCamera->GetForwardVector();

	//get the dot product of the camera forward vector and the velocity
	const float DotProduct = FVector::DotProduct(LastDirectionalJumpDirection, CurrentFloor.HitResult.ImpactNormal);

	//set the movement mode to falling
	SetMovementMode(MOVE_Falling);

	//check if the dot product is less than or equal to 0
	if (DotProduct <= 0)
	{
		//set the velocity
		Velocity += FVector::UpVector * (JumpZVel + JumpBoostAmount) + Velocity.GetSafeNormal() * DirectionalJumpForce;

		//call the blueprint event
		OnCorrectedDirectionalJump.Broadcast(LastDirectionalJumpDirection, Velocity.GetSafeNormal());
	}
	else
	{
		//set the velocity
		Velocity += FVector::UpVector * (JumpZVel + JumpBoostAmount) + LastDirectionalJumpDirection * DirectionalJumpForce;

		//call the blueprint event
		OnDirectionalJump.Broadcast(LastDirectionalJumpDirection);
	}
}

void UPlayerMovementComponent::ToggleRotationMode(bool InValue)
{
	//check if we can't activate the rotation mode
	if (!(bCanActivateRotationMode && bCanEverActivateRotationMode))
	{
		return;
	}

	//set the rotation mode
	bRotationMode = InValue;
}

void UPlayerMovementComponent::StopWallLatch()
{
	//check if the player is not wall latching
	if (!bIsWallLatching)
	{
		return;
	}

	//set is wall latching to false
	bIsWallLatching = false;

	//enable movement
	SetMovementMode(MOVE_Falling);

	//clear the wall latch fall timer handle
	GetWorld()->GetTimerManager().ClearTimer(WallLatchFallTimerHandle);

	//broadcast the blueprint event
	OnWallLatchFall.Broadcast();
}

void UPlayerMovementComponent::LaunchOffWallLatch()
{
	//set is wall latching to false
	bIsWallLatching = false;

	//set last wall latch launch time
	LastWallLatchLaunchTime = GetWorld()->GetTimeSeconds();

	//enable movement
	SetMovementMode(MOVE_Falling);

	//clear the wall latch fall timer handle
	GetWorld()->GetTimerManager().ClearTimer(WallLatchFallTimerHandle);

	//get the player's camera forward vector
	const FVector Forward = PlayerCamera->GetForwardVector();

	//set the direction to launch in
	WallLatchLaunchMovementParams.WorldDirection = Forward;

	//set the strength of the launch to be the larger of the minimum wall latch force and the velocity's length
	WallLatchLaunchMovementParams.Strength = FMath::Max(MinWallLatchForce, Velocity.Length());

	//activate the root motion
	UAsyncRootMovement::AsyncRootMovement(this, this, WallLatchLaunchMovementParams)->Activate();

	//broadcast the blueprint event
	OnWallLatchLaunch.Broadcast();
}

bool UPlayerMovementComponent::DoWallLatch(float DeltaTime)
{

	//do a sphere trace to see if there is a wall near the player
	GetWorld()->SweepSingleByChannel(WallLatchHitResult, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(WallLatchCheckRadius));

	//check if the sphere trace didn't hit anything
	if (!WallLatchHitResult.bBlockingHit)
	{
		return false;
	}

	//check if the actor the trace hits doesn't have the wall latch tag and the component doesn't have the wall latch tag
	if (!WallLatchHitResult.GetActor()->ActorHasTag(WallLatchTag) && !WallLatchHitResult.GetComponent()->ComponentHasTag(WallLatchTag))
	{
		return false;
	}

	//check if we're jumping to prevent the wall latching from activating and overriding the jump
	if (LastWallLatchLaunchTime != 0 && GetWorld()->GetTimeSeconds() > WallLatchCooldown && GetWorld()->GetTimeSeconds() < WallLatchCooldown + LastWallLatchLaunchTime)
	{
		return false;
	}

	//broadcast the blueprint event
	OnWallLatch.Broadcast(WallLatchHitResult);

	//return true
	return true;
}

void UPlayerMovementComponent::CheckForWallRunFinish()
{
	//check if we we're wall running
	if (bIsWallRunning)
	{
		//stop wall running
		bIsWallRunning = false;

		//broadcast the blueprint event
		OnWallRunFinish.Broadcast();
	}
}

bool UPlayerMovementComponent::DoWallRunning(const float DeltaTime)
{
	//do a line trace to see if there is a wall on either side of the player
	GetWorld()->LineTraceSingleByChannel(WallRunningHitResult, GetOwner()->GetActorLocation() + GetOwner()->GetActorRightVector() * WallRunningCheckDistance * -1, GetOwner()->GetActorLocation() + GetOwner()->GetActorRightVector() * WallRunningCheckDistance, ECC_Visibility);

	//check if the line trace didn't hit anything
	if (!WallRunningHitResult.bBlockingHit)
	{
		//check for wall run finish
		CheckForWallRunFinish();

		return false;
	}

	//check if the line trace didn't hit an actor with the wall running tag and the component doesn't have the wall running tag
	if (!WallRunningHitResult.GetActor()->ActorHasTag(WallRunningTag) && !WallRunningHitResult.GetComponent()->ComponentHasTag(WallRunningTag))
	{
		//check for wall run finish
		CheckForWallRunFinish();

		return false;
	}

	//check if we're jumping to prevent the wall running from activating and overriding the jump
	if (LastWallJumpTime != 0 && GetWorld()->GetTimeSeconds() < WallJumpCooldown + LastWallJumpTime)
	{
		//check for wall run finish
		CheckForWallRunFinish();

		return false;
	}

	//get the surface normal of the hit
	const FVector SurfaceNormal = WallRunningHitResult.ImpactNormal;

	//get vector parallel to the surface
	const FVector ParallelVector = FVector::CrossProduct(SurfaceNormal, WallRunningHitResult.GetComponent()->GetUpVector()).GetSafeNormal();

	//get the dot product of the velocity and the parallel vector
	const float DotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), ParallelVector);

	//set the velocity to the parallel vector with the speed and direction we were moving at and add the wall running attach force
	Velocity = ParallelVector.GetSafeNormal() * Velocity.Size() * FMath::Sign(DotProduct) + WallRunningAttachForce * -SurfaceNormal * DeltaTime;

	//broadcast the blueprint event
	OnWallRunStart.Broadcast(WallRunningHitResult);

	return true;
}

void UPlayerMovementComponent::DoWallRunJump(FHitResult InWallHit)
{
	//check if the player is not wall running
	if (!bIsWallRunning)
	{
		return;
	}

	//set is wall running to false
	bIsWallRunning = false;

	//enable movement
	SetMovementMode(MOVE_Falling);

	//get the wall jump's direction
	FVector Direction = InWallHit.ImpactNormal;

	//check if the direction is away from the wall
	if (FVector::DotProduct(Direction, (GetOwner()->GetActorLocation() - InWallHit.Location).GetSafeNormal()) < 0)
	{
		//set the direction to the opposite of the wall jump's direction
		Direction *= -1;
	}

	//set the direction to launch in
	WallRunJumpMovementParams.WorldDirection = Direction;

	//activate the root motion
	UAsyncRootMovement::AsyncRootMovement(this, this, WallRunJumpMovementParams)->Activate();

	//set the wall jump start time
	LastWallJumpTime = GetWorld()->GetTimeSeconds();

	//broadcast the blueprint event
	OnWallRunJump.Broadcast();
}

bool UPlayerMovementComponent::CanJumpAnyway() const
{
	//check if we're wall running or wall latching
	if (bIsWallRunning || bIsWallLatching)
	{
		return true;
	}

	//otherwise return false
	return false;
}


