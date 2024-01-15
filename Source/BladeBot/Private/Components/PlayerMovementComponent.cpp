#include "Components/PlayerMovementComponent.h"
#include "Components/PlayerCameraComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PhysicsVolume.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	bUseFlatBaseForFloorChecks = true;
	bApplyGravityWhileJumping = false;
	MaxWalkSpeed = 1200.f;
	BrakingFrictionFactor = 0.1f;
	JumpZVelocity = 800.f;
	AirControl = 2.f;
	GravityScale = 2.f;
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

	////print whether or not we're exceeding the current max speed
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, IsExceedingMaxSpeed(GetMaxSpeed()) ? FColor::Green : FColor::Red, FString::Printf(TEXT("Exceeding Max Speed: %s"), IsExceedingMaxSpeed(GetMaxSpeed()) ? TEXT("True") : TEXT("False")));

	////print the current max speed
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("Current Max Speed: %f"), GetMaxSpeed()));
}

FVector UPlayerMovementComponent::NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const
{
	//store the result of the parent implementation
	FVector Result = Super::NewFallVelocity(InitialVelocity, Gravity, DeltaTime);

	//check if jump is providing force
	if (GetCharacterOwner()->JumpForceTimeRemaining > 0 && bLastJumpWasDirectional)
	{
		//add the directional jump glide force to the result
		Result += LastDirectionalJumpDirection * DirectionalJumpGlideForce * DeltaTime;

		//print debug message
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Jump Force Time Remaining: %f"), GetCharacterOwner()->JumpForceTimeRemaining));
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

void UPlayerMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	//get the rotation we would be at if we were to be standing on the floor we just landed on
	const FRotator FloorRotation = CurrentFloor.HitResult.ImpactNormal.Rotation();

	//set the rotation to be the floor rotation
	GetOwner()->SetActorRotation(FloorRotation);

	//call the parent implementation
	Super::ProcessLanded(Hit, remainingTime, Iterations);
}

bool UPlayerMovementComponent::DoJump(bool bReplayingMoves)
{
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
	//check if we're in the rotation mode
	if (bRotationMode)
	{
		//rotate the character
		GetOwner()->AddActorWorldRotation(FRotator(GetCharacterOwner()->GetPendingMovementInputVector().X, GetCharacterOwner()->GetPendingMovementInputVector().Y, GetCharacterOwner()->GetPendingMovementInputVector().Z), false, nullptr);

		////set the input modifier to 0 (to disable movement)
		//AnalogInputModifier = 0.f;

		//return zero vector
		return FVector::ZeroVector;
	}

	////reenable movement
	//AnalogInputModifier = 1.f;

	//Store the input vector
	FVector ReturnVec = Super::ConsumeInputVector();

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
		//apply the grapple movement input modifier
		ReturnVec *= GrappleMovementInputModifier;
	}

	//return the return vector
	return ReturnVec;
}

bool UPlayerMovementComponent::ShouldRemainVertical() const
{
	//check if we're in the rotation mode
	if (MovementMode != MOVE_Walking)
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
	if (Hit.ImpactNormal.Z >= 0.7f && IsWalkable(Hit))
	{
		//return true
		return true;
	}

	return false;
}

float UPlayerMovementComponent::GetMinAnalogSpeed() const
{
	return Super::GetMinAnalogSpeed();
}

bool UPlayerMovementComponent::IsExceedingMaxSpeed(float MaxSpeed) const
{
	return Super::IsExceedingMaxSpeed(MaxSpeed);
}

float UPlayerMovementComponent::GetMaxSpeed() const
{
	//Check if the player is grappling
	if (bIsGrappling)
	{
		//return the max speed when grappling
		return GrappleMaxSpeed;
	}
	return Super::GetMaxSpeed();
}

void UPlayerMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
}

float UPlayerMovementComponent::GetMaxAcceleration() const
{
	//Check if the player is grappling
	if (bIsGrappling)
	{
		//return the max acceleration when grappling
		return GrappleMaxAcceleration;
	}

	return Super::GetMaxAcceleration();
}

void UPlayerMovementComponent::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration)
{
	Super::ApplyVelocityBraking(DeltaTime, Friction, BrakingDeceleration);
}

void UPlayerMovementComponent::PhysFlying(float deltaTime, int32 Iterations)
{
	Super::PhysFlying(deltaTime, Iterations);
}

void UPlayerMovementComponent::ApplyAccumulatedForces(float DeltaSeconds)
{
	Super::ApplyAccumulatedForces(DeltaSeconds);
}

void UPlayerMovementComponent::StartGrapple(AGrapplingRopeActor* InGrappleRope)
{
	//check if the player is already grappling
	if (bIsGrappling)
	{
		return;
	}

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

	//call the start grapple event
	OnStartGrapple.Broadcast();
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
	if (IsWalking() && !bUseFlyingMovementMode)
	{
		//set the movement mode to falling to prevent the character from getting stuck on the floor
		SetMovementMode(MOVE_Falling);
	}

	//get the point the character is grappling to
	const FVector GrapplePoint = GrappleRope->GetGrapplePoint(GetCharacterOwner());

	//get the vector from the character to the grapple point
	GrappleDirection = (GrapplePoint - GetCharacterOwner()->GetActorLocation()).GetSafeNormal();

	////storage for the velocity that will be applied from the grapple
	//FVector GrappleVelocity = FVector::ZeroVector;

	//check how we should set the velocity
	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
	switch (GrappleMode)
	{
		case AddToVelocity:
			//add the grapple vector to the character's velocity
			Velocity += GrappleDirection * AddGrappleSpeed * DeltaTime;
			//GrappleVelocity = GrappleDirection * AddGrappleSpeed * DeltaTime;
		break;
		case InterpVelocity:
			// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
			switch (GrappleInterpType)
			{
				case Constant:
					//interpolate the velocity
					Velocity = FMath::VInterpConstantTo(Velocity, GrappleDirection.GetSafeNormal() * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
					//GrappleVelocity = FMath::VInterpConstantTo(Velocity, GrappleDirection.GetSafeNormal() * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
				break;
				case InterpTo:
					//interpolate the velocity
					Velocity = FMath::VInterpTo(Velocity, GrappleDirection.GetSafeNormal() * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
					//GrappleVelocity = FMath::VInterpTo(Velocity, GrappleDirection.GetSafeNormal() * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
				break;
				case InterpStep:
					//interpolate the velocity
					Velocity = FMath::VInterpTo(Velocity, GrappleDirection.GetSafeNormal() * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
					//GrappleVelocity = FMath::VInterpTo(Velocity, GrappleDirection.GetSafeNormal() * InterpGrappleSpeed, DeltaTime, GrappleInterpSpeed);
				break;
			}
		break;
	}

	////get the dot product of the character's velocity and the grapple velocity
	//const float DotProduct = FVector::DotProduct(Velocity.GetSafeNormal(), GrappleVelocity.GetSafeNormal());

	////print the grapple velocity
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("Grapple Velocity: %s"), *GrappleVelocity.ToString()));

	////print the dot product
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("Dot Product: %f"), DotProduct));

	//float Length = GrappleVelocity.Length();

	////check if the dot product is greater than 0.5 or less than or equal to -0.5
	//if (DotProduct > 0.5f || DotProduct <= -0.5f && Length < 10000)
	//{
	//	//apply the grapple velocity
	//	Velocity += GrappleVelocity;
	//}

	////print whether or not we applied the grapple velocity
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, DotProduct > 0.5f || DotProduct <= -0.5f ? FColor::Green : FColor::Red, FString::Printf(TEXT("Applied Grapple Velocity: %s"), DotProduct > 0.5f || DotProduct <= -0.5f ? TEXT("True") : TEXT("False")));

	//update the character's velocity
	UpdateComponentVelocity();

	//check if we're not in the rotation mode
	if (!bRotationMode)
	{
		////rotate the character
		//GetOwner()->AddActorWorldRotation(FRotator(GrappleDirection.X, GrappleDirection.Y, 0.f), false, nullptr);

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
	//set the rotation mode
	bRotationMode = InValue;
}


