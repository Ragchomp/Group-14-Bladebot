#include "Components/PlayerMovementComponent.h"

#include "Characters/PlayerCharacter.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	bUseFlatBaseForFloorChecks = true;
	MaxWalkSpeed = 1200.f;
	BrakingFrictionFactor = 0.1f;
	JumpZVelocity = 1000.f;
	AirControl = 2.f;
}

//void UPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	//call the parent implementation
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	//check if we're not walking on the floor
//	if (!IsWalking())
//	{
//		//clear the bunny hop timer
//		GetWorld()->GetTimerManager().ClearTimer(BunnyHopTimer);
//	}
//}

//bool UPlayerMovementComponent::CanAttemptJump() const
//{
//	//check if the jump type is set to can jump off any surface
//	if (JumpType == CanJumpOffAnySurface && LastHit.HasValidHitObjectHandle())
//	{
//		//check if the current distance from the player to the hit is less than the jump off distance
//		if (const float Distance = FVector::Dist(GetOwner()->GetActorLocation(), LastHit.ImpactPoint); Distance <= JumpOffDistance)
//		{
//			//set whether or not the player is jumping off of something
//			bIsWallJumping = true;
//
//			//return true
//			return true;
//		}
//	}
//	
//	//otherwise return whether both the parent implementation returns true and the player is not falling
//	return Super::CanAttemptJump() && !IsFalling();
//}

//void UPlayerMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
//{
//	//call the parent implementation
//	Super::HandleImpact(Hit, TimeSlice, MoveDelta);
//
//	//set the last hit
//	LastHit = Hit;
//}

bool UPlayerMovementComponent::DoJump(bool bReplayingMoves)
{
	switch (JumpType)
	{
		case Normal:
		break;
		case AlwaysBoosted:
			//do a boosted jump
			BoostJump(JumpZVelocity);

			//return true
			return true;
		//case CanJumpOffAnySurface:
		//	//check if the player is wall jumping
		//	if (bIsWallJumping)
		//	{
		//		//check if the wall jump was successful
		//		if (WallJump())
		//		{
		//			//return true
		//			return true;
		//		}
		//	}
		//break;
		//case BunnyHop:
		//	//check if we're moving fast enough to bunny hop
		//	if (Velocity.Length() >= MinBunnyHopSpeed)
		//	{
		//		//do a boosted jump
		//		BoostJump(BunnyHopJumpZVal);

		//		//set the max movement speed to be the bunny hop max speed
		//		bIsBunnyHopping = true;

		//		//return true
		//		return true;
		//	}
		//break;
		//case BoostedWhenAtLedgeAndMovingTowardsLedge:
		//	//check if we're walking
		//	if (MovementMode == MOVE_Walking && Velocity.Length() >= 10)
		//	{
		//		//find the floor result
		//		FFindFloorResult FloorResult;
		//		FindFloor(GetOwner()->GetActorLocation(), FloorResult, false);

		//		//check if the floor result is valid
		//		if (FloorResult.HitResult.IsValidBlockingHit())
		//		{
		//			//get the floor location
		//			FVector FloorLocation = FloorResult.HitResult.Location;

		//			//get the location to check for a ledge jump
		//			FVector CheckLocation = FloorLocation + (Velocity.GetSafeNormal() * LedgeJumpDistance);

		//			//get the closest point on the floor to the check location
		//			FVector ClosestPoint;
		//			FloorResult.HitResult.GetComponent()->GetClosestPointOnCollision(CheckLocation, ClosestPoint);

		//			//get the distance to the check location
		//			const float CheckDistance = FVector::Dist(CheckLocation, FloorLocation);

		//			//get the distance to the closest point
		//			const float ClosestDistance = FVector::Dist(ClosestPoint, FloorLocation);

		//			//check if the distance to the check location is greater than the distance to the closest point
		//			if (CheckDistance > ClosestDistance)
		//			{
		//				//do a boosted jump
		//				BoostJump(JumpZVelocity);

		//				//return true
		//				return true;
		//			}
		//		}
		//		
		//	}
		//break;
		case BoostedWhenMovingFast:
			//check if we're moving fast enough to bunny hop
			if (Velocity.Length() >= MinSpeedForSpeedBoost)
			{
				//do a boosted jump
				BoostJump(JumpZVelocity);

				//return true
				return true;
			}
		break;
		default: ;
	}

	//return the result of the parent implementation
	return Super::DoJump(bReplayingMoves);
}

//bool UPlayerMovementComponent::IsExceedingMaxSpeed(float MaxSpeed) const
//{
	////check if the player is bunny hopping
	//if (bIsBunnyHopping)
	//{
	//	//if so, return whether or not the current velocity is greater than the max bunny hop speed
	//	return false;
	//}
	//
	//return the result of the parent implementation
	//return Super::IsExceedingMaxSpeed(MaxSpeed);
//}

//void UPlayerMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
//{
	////check if the jump type is set to bunny hop
	//if (JumpType == BunnyHop)
	//{
	//	//check if the current velocity is greater than the minimum bunny hop speed
	//	if (Velocity.Size() >= MinBunnyHopSpeed)
	//	{
	//		//set the max movement speed to be the bunny hop max speed
	//		bIsBunnyHopping = true;
	//	}
	//	else
	//	{
	//		//reset is bunny hopping
	//		bIsBunnyHopping = false;
	//
	//		//clear the bunny hop timer
	//		GetWorld()->GetTimerManager().ClearTimer(BunnyHopTimer);
	//	}
	//}
	//
	////call the parent implementation
	//Super::ProcessLanded(Hit, remainingTime, Iterations);
	//
	////set a timer for the bunny hop
	//GetWorld()->GetTimerManager().SetTimer(BunnyHopTimer, this, &UPlayerMovementComponent::StopBunnyHop, BunnyHopTime, false);
//}

//void UPlayerMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
//{
//	//check if the player is bunny hopping
//	if (bIsBunnyHopping)
//	{
//		//call the parent implementation with the bunny hop braking deceleration
//		Super::CalcVelocity(DeltaTime, 0, bFluid, BunnyHopBrakingDeceleration);
//	}
//	else
//	{
//		//call the parent implementation
//		Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
//	}
//}

//void UPlayerMovementComponent::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration)
//{
//	//check if the player is bunny hopping
//	if (bIsBunnyHopping)
//	{
//		//call the parent implementation with the bunny hop braking deceleration
//		Super::ApplyVelocityBraking(DeltaTime, Friction, BunnyHopBrakingDeceleration);
//	}
//	else
//	{
//		//call the parent implementation
//		Super::ApplyVelocityBraking(DeltaTime, Friction, BrakingDeceleration);
//	}
//}

void UPlayerMovementComponent::PhysFlying(const float DeltaTime, const int32 Iterations)
{
	//check if the player is grappling
	if (bIsGrappling)
	{
		//set the grapple velocity
		UpdateGrappleVelocity(DeltaTime);
	}

	//call the parent implementation
	Super::PhysFlying(DeltaTime, Iterations);
}

FVector UPlayerMovementComponent::ConsumeInputVector()
{
	//Store the input vector
	const FVector ReturnVec = Super::ConsumeInputVector();

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

	//check if the player is grappling
	if (bIsGrappling)
	{
		//return the input vector multiplied by the grapple input modifier
		return ReturnVec * GrappleMovementInputModifier;
	}

	return ReturnVec;
}

bool UPlayerMovementComponent::HandlePendingLaunch()
{
	//store the result of the parent implementation
	const bool ReturnVal = Super::HandlePendingLaunch();

	//check if the player is grappling
	if (bIsGrappling)
	{
		//set the movement mode back to flying
		SetMovementMode(MOVE_Flying);
	}

	return ReturnVal;
}

void UPlayerMovementComponent::StartGrapple(AGrapplingRopeActor* GrappleRope)
{
	//check if the player is already grappling
	if (!bIsGrappling)
	{
		//set is grappling to true
		bIsGrappling = true;

		//set the grapple object
		GrappleObject = GrappleRope;

		//set the movement mode to flying so we don't get stuck on the floor
		SetMovementMode(MOVE_Flying);
	}
}

void UPlayerMovementComponent::StopGrapple()
{
	if (bIsGrappling)
	{
		//set is grappling to false
		bIsGrappling = false;

		//reset the movement mode
		SetDefaultMovementMode();

		//reset the character's rotation
		GetCharacterOwner()->SetActorRotation(FRotator(0.f, 0, 0.f));
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
	//get the player controller
	const APlayerController* PC = GetOwner()->GetNetOwningPlayer()->GetPlayerController(GetWorld());

	//get the camera location and rotation
	FVector CameraLocation;
	FRotator CameraRotation;

	//set the camera location and rotation
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

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

void UPlayerMovementComponent::UpdateGrappleVelocity(const float DeltaTime)
{
	//get the point the character is grappling to
	const FVector GrapplePoint = GrappleObject->GetGrapplePoint(GetCharacterOwner());

	//get the vector from the character to the grapple point
	GrappleDirection = (GrapplePoint - GetCharacterOwner()->GetActorLocation()).GetSafeNormal();

	//check if we should set the velocity
	switch (GrappleMode)
	{
		case SetVelocity:
			//set the velocity
			Velocity = GrappleDirection * SetGrappleSpeed;
		break;
		case AddToVelocity:
			//add the grapple vector to the character's velocity
			Velocity += GrappleDirection * AddGrappleSpeed * DeltaTime;
		break;
		case InterpVelocity:
			switch (GrappleInterpType)
			{
				case Constant:
					//interpolate the velocity
					Velocity = FMath::VInterpConstantTo(Velocity, GrappleDirection.GetSafeNormal() * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
				case InterpTo:
					//interpolate the velocity
					Velocity = FMath::VInterpTo(Velocity, GrappleDirection.GetSafeNormal() * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
				case InterpStep:
					//interpolate the velocity
					Velocity = FMath::VInterpTo(Velocity, GrappleDirection.GetSafeNormal() * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
			}
		break;
	}

	//update the character's velocity
	UpdateComponentVelocity();

	//set the character's rotation to face the grapple point
	GetCharacterOwner()->SetActorRotation(GrappleDirection.Rotation());
}

bool UPlayerMovementComponent::WallJump()
{
	//check if the last hit is valid
	if (LastHit.HasValidHitObjectHandle())
	{
		//get the normal of the hit
		const FVector Normal = LastHit.Normal;

		//get the forward vector of the character
		const FVector Forward = GetCharacterOwner()->GetActorForwardVector();

		//get the dot product of the normal and the forward vector
		const float Dot = FVector::DotProduct(Normal, Forward);

		//check if the angle between the normal and the forward vector is less than 90 degrees
		if (const float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot)); Angle < 90.f)
		{
			//set the velocity
			Velocity = FVector::UpVector * JumpZVelocity + GetOwner()->GetActorForwardVector() * DirectionalJumpForce;

			//add the wall jump force to the velocity
			Velocity += Normal * WallJumpForce;

			//set the movement mode to falling
			SetMovementMode(MOVE_Falling);

			//reset is wall jumping and the last hit
			bIsWallJumping = false;
			LastHit = FHitResult();

			//return true
			return true;
		}
	}

	//return false
	return false;
}

//void UPlayerMovementComponent::StopBunnyHop() const
//{
//	//set the max movement speed to be the default max walk speed
//	bIsBunnyHopping = false;
//}

void UPlayerMovementComponent::BoostJump(const float JumpZVel)
{
	switch (JumpBoostType)
	{
		case NoBoost:
			//set the velocity
			Velocity += FVector::UpVector * JumpZVel;
		break;
		case AddToZ:
			//add the velocity
			Velocity += FVector::UpVector * (JumpZVel + JumpBoostAmount);
		break;
		case SetZ:
			//set the velocity
			Velocity = FVector::UpVector * (JumpZVel + JumpBoostAmount);
		break;
		case DirectionalJump:
			//set the velocity
			Velocity += FVector::UpVector * (JumpZVel + JumpBoostAmount) + GetOwner()->GetActorForwardVector() * DirectionalJumpForce;
		break;
		//case DirectionalJumpNoBoost:
		//	//set the velocity
		//	Velocity = FVector::UpVector * JumpZVel + GetOwner()->GetActorForwardVector() * DirectionalJumpForce;
		//break;
		//case DirectionalJumpNoZ:
		//	//set the velocity
		//	Velocity = GetOwner()->GetActorForwardVector() * DirectionalJumpForce;
		default: ;
	}

	//set the movement mode to falling
	SetMovementMode(MOVE_Falling);
}

