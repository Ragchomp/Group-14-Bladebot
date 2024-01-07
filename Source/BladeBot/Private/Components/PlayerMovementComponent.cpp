#include "Components/PlayerMovementComponent.h"
#include "Components/PlayerCameraComponent.h"
#include "Characters/PlayerCharacter.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	//bUseFlatBaseForFloorChecks = true;
	MaxWalkSpeed = 1200.f;
	BrakingFrictionFactor = 0.1f;
	JumpZVelocity = 1000.f;
	AirControl = 2.f;
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
	//get the current speed
	OriginalSpeed = Velocity.Size();

	//call the parent implementation
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//check if the player is grappling
	if (bIsGrappling)
	{
		//set the grapple velocity
		UpdateGrappleVelocity(DeltaTime);
	}

	//check if we can wall jump and the player is falling
	if (CanWallJump())
	{
		//call the blueprint event
		//OnWallJump();
		OnCanWallJump.Broadcast(LastHit);
	}
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

bool UPlayerMovementComponent::CanAttemptJump() const
{
	//check if we can attempt a jump and the player is falling
	if (CanWallJump())
	{
		//return whether the player can wall jump
		return true;
	}

	//otherwise return whether both the parent implementation returns true and the player is not falling
	return Super::CanAttemptJump() && !IsFalling();
}

void UPlayerMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	//call the parent implementation
	Super::HandleImpact(Hit, TimeSlice, MoveDelta);

	//check if we're falling
	if (IsFalling())
	{
		//set the last hit
		LastHit = Hit;

		//set can wall jump to true
		bCanWallJump = true;

		//clear the wall jump timer
		GetWorld()->GetTimerManager().ClearTimer(WalljumpTimerHandle);

		//set the wall jump timer
		GetWorld()->GetTimerManager().SetTimer(WalljumpTimerHandle, this, &UPlayerMovementComponent::DisableWallJump, WallJumpTime, false);
	}
}

void UPlayerMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	//call the parent implementation
	Super::ProcessLanded(Hit, remainingTime, Iterations);

	//check if we can wall jump
	if (bCanWallJump)
	{
		//clear the wall jump timer
		GetWorld()->GetTimerManager().ClearTimer(WalljumpTimerHandle);

		//set bCanWallJump to false
		bCanWallJump = false;

		//reset the last hit
		LastHit = FHitResult();
	}
}

bool UPlayerMovementComponent::DoJump(bool bReplayingMoves)
{
	//check if we can wall jump
	if (CanWallJump())
	{
		//do a wall jump
		DoWallJump();

		//return true
		return true;
	}

	//check if we're moving fast enough to do a boosted jump and we're on the ground
	if (Velocity.Length() >= MinSpeedForSpeedBoost && !IsFalling())
	{
		//do a boosted jump
		BoostJump(JumpZVelocity);

		//return true
		return true;
	}

	//store whether or not a normal jump was successful
	const bool bNormalJump = Super::DoJump(bReplayingMoves);

	//check if the normal jump was successful
	if (bNormalJump)
	{
		//call the blueprint event
		OnNormalJump.Broadcast();
	}

	//return the result of the parent implementation
	return bNormalJump;
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

float UPlayerMovementComponent::GetMaxAcceleration() const
{
	//Check if the player is grappling
	if (bIsGrappling)
	{
		//return the max acceleration when grappling
		return 4000;
	}

	return Super::GetMaxAcceleration();
}

void UPlayerMovementComponent::StartGrapple(AGrapplingRopeActor* InGrappleRope)
{
	//check if the player is already grappling
	if (!bIsGrappling)
	{
		//set is grappling to true
		bIsGrappling = true;

		//set the grapple object
		GrappleRope = InGrappleRope;

		//check if we should use the flying movement mode
		if (bUseFlyingMovementMode)
		{
			//set the movement mode to flying
			SetMovementMode(MOVE_Flying);
		}
		else
		{
			//set the gravity scale to 0
			GravityScale = 0.f;
		}
	}
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
		else
		{
			//reset the gravity scale
			GravityScale = 1.f;
		}

		//check if we should apply a speed boost
		if (bEndGrappleSpeedBoost)
		{
			//check if we should apply the boost in the direction the player is looking
			if (bEndGrappleBoostInLookDirection)
			{
				//get the player's camera forward vector
				const FVector Forward = PlayerCamera->GetForwardVector();

				//add the boost amount to the velocity
				Velocity += Forward * EndGrappleBoostAmount;
			}
			else
			{
				//add the boost amount to the velocity
				Velocity += Velocity.GetSafeNormal() * EndGrappleBoostAmount;
			}
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
	//check if we're on the ground
	if (IsWalking())
	{
		//set the movement mode to falling to prevent the character from getting stuck on the floor
		SetMovementMode(MOVE_Falling);
	}

	//get the point the character is grappling to
	const FVector GrapplePoint = GrappleRope->GetGrapplePoint(GetCharacterOwner());

	//get the vector from the character to the grapple point
	GrappleDirection = (GrapplePoint - GetCharacterOwner()->GetActorLocation()).GetSafeNormal();

	//check if we should set the velocity
	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
	switch (GrappleMode)
	{
		case AddToVelocity:
			//add the grapple vector to the character's velocity
			Velocity += GrappleDirection * AddGrappleSpeed * DeltaTime;
		break;
		case InterpVelocity:
			// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
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

	////get the position the character will be at after moving with the current velocity
	//const FVector FutureLoc = GetOwner()->GetActorLocation() + Velocity * DeltaTime;

	////get the vector from the grapple point to the character's future location (change this to be clamped to the length of the rope)
	//const FVector GrappleToFutureLoc = (GrapplePoint - FutureLoc).GetSafeNormal();

	////get the clamped position
	//const FVector ClampedPos = GrapplePoint - GrappleToFutureLoc;

	////get the vector from the current location to the clamped position
	//const FVector CurrentToClamped = (ClampedPos - GetOwner()->GetActorLocation()).GetSafeNormal();

	////get the angle between the current velocity and the current to clamped vector
	//const float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Velocity.GetSafeNormal(), CurrentToClamped)));

	////get the cross product of the current velocity and the grapple direction
	//const FVector CrossProduct = FVector::CrossProduct(Velocity.GetSafeNormal(), GrappleDirection);

	////draw debug arrow
	//DrawDebugDirectionalArrow(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + CrossProduct * 100.f, 100.f, FColor::Blue, false, 0.0f, 0, 5.f);

	////rotate the velocity by the cross product
	//Velocity = Velocity.RotateAngleAxis(Angle, CrossProduct);

	//check if the character's speed has decreased
	if (Velocity.Size() < OriginalSpeed)
	{
		//set the velocity to the original speed
		Velocity = Velocity.GetSafeNormal() * OriginalSpeed;
	}

	//update the character's velocity
	UpdateComponentVelocity();

	//set the character's rotation to face the grapple point
	GetCharacterOwner()->SetActorRotation(GrappleDirection.Rotation());
}

bool UPlayerMovementComponent::CanWallJump() const
{
	//check if we can wall jump, we're in the air and the last hit is valid
	if (bCanWallJump && IsFalling() && LastHit.IsValidBlockingHit())
	{
		//return true
		return true;
	}


	//otherwise return false
	return false;
}

void UPlayerMovementComponent::DoWallJump()
{
	//get the normal of the hit
	const FVector Normal = LastHit.Normal;

	//check if we should scale the wall jump force by the player's velocity
	if (bScaleWallJumpForceByVelocity)
	{
		//scale the wall jump force by the player's velocity
		Velocity += Velocity.GetSafeNormal() * Velocity.Size() * WallJumpForceVelocityScale + FVector::ZAxisVector * WallJumpZVel;
	}
	else
	{
		//add the wall jump force to the velocity
		Velocity += Normal * WallJumpForce + FVector::ZAxisVector * WallJumpZVel;
	}

	//call the blueprint event
	//OnWallJump();
	OnWallJump.Broadcast(LastHit);
}

void UPlayerMovementComponent::DisableWallJump()
{
	//set can wall jump to false
	bCanWallJump = false;

	//reset the last hit
	LastHit = FHitResult();
}

void UPlayerMovementComponent::BoostJump(const float JumpZVel)
{
	//get the direction of the jump
	const FVector Direction = PlayerCamera->GetForwardVector();

	//get the dot product of the camera forward vector and the velocity
	const float DotProduct = FVector::DotProduct(Direction, CurrentFloor.HitResult.ImpactNormal);

	//set the movement mode to falling
	SetMovementMode(MOVE_Falling);

	//check if the dot product is less than or equal to 0
	if (DotProduct <= 0)
	{
		//set the velocity
		Velocity += FVector::UpVector * (JumpZVel + JumpBoostAmount) + Velocity.GetSafeNormal() * DirectionalJumpForce;

		//call the blueprint event
		OnCorrectedDirectionalJump.Broadcast(Direction, Velocity.GetSafeNormal());

		//debug message
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Corrected: %f"), DotProduct));
	}
	else
	{
		//set the velocity
		Velocity += FVector::UpVector * (JumpZVel + JumpBoostAmount) + Direction * DirectionalJumpForce;

		//call the blueprint event
		OnDirectionalJump.Broadcast(Direction);
	}
}

