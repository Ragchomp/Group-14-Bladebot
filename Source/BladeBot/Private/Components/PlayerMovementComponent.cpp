#include "Components/PlayerMovementComponent.h"

#include "Characters/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
}

void UPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//call the parent implementation
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//print the current movement mode
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("Movement Mode: %s"), *GetMovementName()));
}

void UPlayerMovementComponent::PhysFalling(float DeltaTime, int32 Iterations)
{
	//check the distance to the nearest floor
	FHitResult FloorHit;
	GetWorld()->LineTraceSingleByChannel(FloorHit, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() - FVector::UpVector * FloorCheckDistance, ECC_Visibility);

	//check if there is a floor within that distance
	if (FloorHit.bBlockingHit)
	{
		//check if the distance to the floor is less than the capsule half height + 1
		if (const float DistanceToFloor = FVector::Dist(GetOwner()->GetActorLocation(), FloorHit.ImpactPoint); DistanceToFloor < GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 1.f)
		{
			//process the landing
			ProcessLanded(FloorHit, 0.f, Iterations);
			return;
		}
	}

	//call the parent implementation
	Super::PhysFalling(DeltaTime, Iterations);
}

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
	//check if the player is grappling
	if (bIsGrappling)
	{
		//add the input vector to the grapple input vector
		GrappleInputVector += GetPendingInputVector();

		//clamp the grapple input vector
		GrappleInputVector = GrappleInputVector.GetClampedToMaxSize(1.f);
	}

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

	return ReturnVec;
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
	}
}

bool UPlayerMovementComponent::CanGrapple() const
{
	//do a line trace to see if the player is aiming at something within grapple range
	FHitResult GrappleHit;
	GrappleLineTrace(GrappleHit, MaxGrappleDistance);

	//if the line trace hit something, return true
	if (GrappleHit.bBlockingHit)
	{
		return true;
	}
	
	//otherwise return false
	return false;
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
	GrappleVelocity = (GrapplePoint - GetCharacterOwner()->GetActorLocation()).GetSafeNormal();

	//check if we should set the velocity
	switch (GrappleMode)
	{
		case SetVelocity:
			//set the velocity
			Velocity = GrappleVelocity * SetGrappleSpeed;
		break;
		case AddToVelocity:
			//add the grapple vector to the character's velocity
			Velocity += GrappleVelocity * AddGrappleSpeed * DeltaTime;
		break;
		case InterpVelocity:
			switch (GrappleInterpType)
			{
				case Constant:
					//interpolate the velocity
					Velocity = FMath::VInterpConstantTo(Velocity, GrappleVelocity * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
				case InterpTo:
					//interpolate the velocity
					Velocity = FMath::VInterpTo(Velocity, GrappleVelocity * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
				case InterpStep:
					//interpolate the velocity
					Velocity = FMath::VInterpTo(Velocity, GrappleVelocity * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
			}
		break;
	}

	
	//update the character's velocity
	UpdateComponentVelocity();
}

void UPlayerMovementComponent::StopSliding()
{
	//set the movement mode to the default land movement mode
	SetMovementMode(DefaultLandMovementMode);
}

