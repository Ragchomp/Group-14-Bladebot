#include "Components/PlayerMovementComponent.h"
#include "Components/PlayerCameraComponent.h"
#include "Characters/PlayerCharacter.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	bUseFlatBaseForFloorChecks = true;
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
	//call the parent implementation
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//check if the player is grappling
	if (bIsGrappling)
	{
		//set the grapple velocity
		UpdateGrappleVelocity(DeltaTime);
	}

	////debug arrows to show a theortical wall jump
	//DrawDebugDirectionalArrow(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.f, 100.f, FColor::Red, false, 0.1f, 0, 5.f);
	//DrawDebugDirectionalArrow(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + GetOwner()->GetActorRightVector() * 100.f, 100.f, FColor::Green, false, 0.1f, 0, 5.f);
	//DrawDebugDirectionalArrow(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + FVector::CrossProduct(GetOwner()->GetActorForwardVector(), GetOwner()->GetActorRightVector()) * 100.f, 100.f, FColor::Blue, false, 0.1f, 0, 5.f);
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
	////call the parent implementation
	//Super::HandleImpact(Hit, TimeSlice, MoveDelta);

	////check if we're falling
	//if (IsFalling())
	//{
	//	//set the last hit
	//	LastHit = Hit;

	//	//set can wall jump to true
	//	bCanWallJump = true;

	//	//clear the wall jump timer
	//	GetWorld()->GetTimerManager().ClearTimer(WalljumpTimerHandle);

	//	//set the wall jump timer
	//	GetWorld()->GetTimerManager().SetTimer(WalljumpTimerHandle, this, &UPlayerMovementComponent::DisableWallJump, WallJumpTime, false);
	//}
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

	//return the result of the parent implementation
	return Super::DoJump(bReplayingMoves);
}

//void UPlayerMovementComponent::PhysFlying(const float DeltaTime, const int32 Iterations)
//{
//	//check if the player is grappling
//	if (bIsGrappling)
//	{
//		//set the grapple velocity
//		UpdateGrappleVelocity(DeltaTime);
//	}
//
//	//call the parent implementation
//	Super::PhysFlying(DeltaTime, Iterations);
//}

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
		//SetMovementMode(MOVE_Flying);
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
		//SetMovementMode(MOVE_Flying);

		//launch the character towards the grapple point
		//Launch(GrappleObject->GetGrapplePoint(GetCharacterOwner()) - GetCharacterOwner()->GetActorLocation().GetSafeNormal() * GrappleLaunchForce);

		//check if we have a valid movement base
		//if (GetMovementBase()->IsValidLowLevel())
		//{
		//	//jump off the movement base to prevent the character from getting stuck on the floor
		//	JumpOff(GetMovementBase()->GetOwner());	
		//}
	}
}

void UPlayerMovementComponent::StopGrapple()
{
	if (bIsGrappling)
	{
		//set is grappling to false
		bIsGrappling = false;

		//reset the movement mode
		//SetDefaultMovementMode();

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
	//check if we're on the ground
	if (IsWalking())
	{
		//set the movement mode to falling to prevent the character from getting stuck on the floor
		SetMovementMode(MOVE_Falling);
	}

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

bool UPlayerMovementComponent::IsFacingNearWall() const
{
	//check if the player can wall jump right now
	if (bCanWallJump)
	{
		//check if the last hit is valid
		if (LastHit.IsValidBlockingHit())
		{
			//get the normal of the hit
			const FVector Normal = LastHit.Normal;

			//get the upwards vector of the character
			const FVector Up = GetCharacterOwner()->GetActorUpVector();

			//get the cross product of the normal and the upwards vector
			const FVector Cross = FVector::CrossProduct(Normal, Up);

			//check if the dot product of the cross product and the forward vector is greater than the WallJumpDotModifier
			if (const float Dot = FVector::DotProduct(Cross, GetOwner()->GetActorForwardVector()); Dot > WallJumpDotModifier)
			{
				//return true
				return true;
			}
		}
	}

	//return false
	return false;
}

bool UPlayerMovementComponent::CanWallJump() const
{
	//check if we can wall jump, we're in the air and the last hit is valid
	if (bCanWallJump && IsFalling() && LastHit.IsValidBlockingHit())
	{
		//check if the player is facing near enough to the wall or if we don't care about the player facing the wall
		if (IsFacingNearWall() || !bMustFaceWallToWallJump)
		{
			//return true
			return true;
		}
	}

	return false;
}

void UPlayerMovementComponent::DoWallJump()
{
	//get the normal of the hit
	const FVector Normal = LastHit.Normal * WallJumpForceMultipliers.X;

	////get the upwards vector of the character
	//const FVector Up = PlayerCamera->GetUpVector();

	//get the forward vector of the character
	const FVector Forward = PlayerCamera->GetForwardVector() * WallJumpForceMultipliers.Y;

	////get the cross product of the normal and the upwards vector
	//const FVector Cross = FVector::CrossProduct(Normal, Up);

	//get the cross product of the normal and the forward vector
	const FVector Cross = FVector::CrossProduct(LastHit.Normal, PlayerCamera->GetForwardVector()).GetSafeNormal() * WallJumpForceMultipliers.Z;

	//get the direction to jump in
	const FVector JumpDirection = (Normal + Forward + Cross).GetSafeNormal() * WallJumpForce;

	//set the velocity (change this to rotate the velocity instead of setting it later)
	Velocity = JumpDirection;
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
			Velocity += FVector::UpVector * (JumpZVel + JumpBoostAmount) + PlayerCamera->GetForwardVector() * DirectionalJumpForce;
		break;
		default: ;
	}

	//set the movement mode to falling
	SetMovementMode(MOVE_Falling);
}

