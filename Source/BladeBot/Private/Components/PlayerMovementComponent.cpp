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

	//check if the player is grappling
	if (bIsGrappling)
	{
		//set the grapple velocity
		UpdateGrappleVelocity(DeltaTime);
	}

	////print whether the player is grappling or not
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("Is Grappling: %s"), bIsGrappling ? TEXT("True") : TEXT("False")));

	////print the current movement mode
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("Movement Mode: %s"), *GetMovementName()));

	//check the distance to the nearest floor
	FHitResult FloorHit;
	GetWorld()->LineTraceSingleByChannel(FloorHit, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() - FVector::UpVector * 1000.f, ECC_Visibility);

	//check if there is a floor
	if (FloorHit.bBlockingHit)
	{
		const float DistanceToFloor = FVector::Dist(GetOwner()->GetActorLocation(), FloorHit.ImpactPoint);

		//check if the distance to the floor is less than the capsule half height + 1
		if (DistanceToFloor < GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 1.f)
		{
			//set the movement mode to walking
			SetMovementMode(DefaultLandMovementMode);
		}

		//print debug message
		///GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("Floor Distance: %f"), DistanceToFloor));
	}
}

void UPlayerMovementComponent::PhysFlying(float deltaTime, int32 Iterations)
{
	//store the analog input modifier
	const float OldAnalogInputModifier = AnalogInputModifier;

	//check if the player is grappling
	if (bIsGrappling)
	{
		//disable input
		AnalogInputModifier = 0;
	}

	//call the parent implementation
	Super::PhysFlying(deltaTime, Iterations);

	//restore the analog input modifier
	AnalogInputModifier = OldAnalogInputModifier;
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

		////print debug message
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Start Grapple"));
	}
}

void UPlayerMovementComponent::StopGrapple()
{
	if (bIsGrappling)
	{
		//set is grappling to false
		bIsGrappling = false;

		//check if the player is still in the flying movement mode
		if (MovementMode == MOVE_Flying)
		{
			//set the movement mode to the default movement mode
			SetDefaultMovementMode();
		}

		////print debug message
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Stop Grapple"));
	}
}

bool UPlayerMovementComponent::CanGrapple() const
{
	//do a line trace to see if the player is aiming at something within grapple range
	FHitResult GrappleHit;
	GrappleLineTrace(GrappleHit);

	//if the line trace hit something, return true
	if (GrappleHit.bBlockingHit)
	{
		return true;
	}

	//otherwise return false
	return false;
}

void UPlayerMovementComponent::GrappleLineTrace(FHitResult& OutHit) const
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
	const FVector End = CameraLocation + Rotation * MaxGrappleDistance;

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

void UPlayerMovementComponent::UpdateGrappleVelocity(float DeltaTime)
{
	//get the point the character is grappling to
	const FVector GrapplePoint = GrappleObject->GetGrapplePoint(GetCharacterOwner());

	//get the vector from the character to the grapple point
	GrappleVelocity = (GrapplePoint - GetCharacterOwner()->GetActorLocation()).GetSafeNormal();

	//check if we should set the velocity
	if (bSetVelocity)
	{
		//set the velocity
		Velocity = GrappleVelocity * SetGrappleSpeed;
	}
	else
	{
		//add the grapple vector to the character's velocity
		Velocity += GrappleVelocity * AddGrappleSpeed * DeltaTime;	
	}

	//update the character's velocity
	UpdateComponentVelocity();
}

