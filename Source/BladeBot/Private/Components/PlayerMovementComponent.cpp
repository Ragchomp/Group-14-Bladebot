#include "Components/PlayerMovementComponent.h"

#include "Characters/PlayerCharacter.h"
#include "GrapplingHook/GrapplingHookHead.h"
#include "HUD/PlayerOverlay.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
}

void UPlayerMovementComponent::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//get the player character
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
}

void UPlayerMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	//call the parent implementation
	Super::PhysCustom(DeltaTime, Iterations);

	//call the appropriate physics function depending on the custom movement mode
	switch (CustomMovementMode)
	{
		case CCM_Grappling:
			PhysGrappling(DeltaTime, Iterations);
			break;
		default: ;
	}
}

void UPlayerMovementComponent::StartGrapple(AActor* GrappleRope)
{
	//if the player is already grappling, don't do anything
	if (MovementMode == MOVE_Custom && CustomMovementMode == CCM_Grappling)
	{
		return;
	}

	//set the movement mode to custom and set the custom movement mode to grappling
	//MovementMode = MOVE_Custom;
	//CustomMovementMode = CCM_Grappling;

	//vlerp the characters velocity to the vector applied by grappling so as to help improve game feel
	//UKismetMathLibrary::VLerp()
}

void UPlayerMovementComponent::StopGrapple()
{
	//if the player is not grappling, don't do anything
	if (MovementMode != MOVE_Custom || CustomMovementMode != CCM_Grappling)
	{
		return;
	}

	//set the movement mode to falling
	MovementMode = MOVE_Falling;
	CustomMovementMode = CCM_None;
}

bool UPlayerMovementComponent::CanGrapple()
{
	//if the player overlay is invalid, don't do anything
	if (PlayerCharacter->PlayerOverlay == nullptr)
	{
		return false;
	}

	//check if the grapple hook is retracted
	if (GrapplingHookRef->GrappleState == EGrappleState::EGS_Retracted)
	{
		//do a line trace to see if the player is aiming at something within grapple range
		GrappleLineTrace(GrappleHit);

		//if the line trace hit something, return true
		if (GrappleHit.bBlockingHit)
		{
			//enable the grappling crosshair
			PlayerCharacter->PlayerOverlay->EnableGrapplingCrosshair(true);
			return true;
		}
	}

	PlayerCharacter->PlayerOverlay->EnableGrapplingCrosshair(false);
	return false;
}

void UPlayerMovementComponent::GrappleLineTrace(FHitResult& OutHit)
{
	//get the camera location and rotation
	FVector CameraLocation;
	FRotator CameraRotation;


	//get the player controller
	const APlayerController* PC = GetOwner()->GetNetOwningPlayer()->GetPlayerController(GetWorld());

	//get the camera location and rotation
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	//get the forward vector of the camera rotation
	const FVector Rotation = CameraRotation.Quaternion().GetForwardVector();

	//get the end point of the line trace
	const FVector End = CameraLocation + Rotation * GrappleMaxDistance;

	//set the collision params
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	//do the line trace
	GetWorld()->LineTraceSingleByChannel(OutHit, CameraLocation, End, ECC_Visibility, CollisionParams);
}

void UPlayerMovementComponent::SetGrappleVelocity(float DeltaTime)
{
	//get the point the character is grappling to
	const FVector GrapplePoint = GrappleObject->GetGrapplePoint(GetCharacterOwner());

	//get the vector from the character to the grapple point
	const FVector GrappleVector = GrapplePoint - GetCharacterOwner()->GetActorLocation();

	//get the velocity of the character
	FVector CharVelocity = GetCharacterOwner()->GetVelocity();

	//get the magnitude of the velocity
	float VelocityMagnitude = CharVelocity.Size();

	//get the equilibrium velocity of the character's velocity and the grapple vector and the centrifugal force
	FVector EquilibriumVelocity = (CharVelocity + GetCentrifugalForce(CharVelocity, GetCharacterOwner()->GetActorLocation(), GrapplePoint) + GrappleVector.GetSafeNormal() * PullStrenght * DeltaTime);

	//set the velocity of the character
	GetCharacterOwner()->LaunchCharacter(EquilibriumVelocity * VelocityMagnitude * DeltaTime, false, false);
}

void UPlayerMovementComponent::PhysGrappling(float DeltaTime, int32 Iterations)
{
	//if the player is trying to reel in the grappling hook, pull the player towards the grappling hook
	if (GrappleObject)
	{
		SetGrappleVelocity(DeltaTime);
	}
}

FVector UPlayerMovementComponent::GetCentrifugalForce(FVector InVelocity, FVector InGrappleLocation, FVector InGrappleHitLocation)
{
	//get the path of inertia
	FVector PathOfInertia = InVelocity * InVelocity.Size();

	//get the vector from the grapple location to the grapple hit location
	FVector GrappleVector = InGrappleHitLocation - InGrappleLocation;

	//get the radius of the circle
	float Radius = GrappleVector.Size();

	//get the angular velocity
	FVector AngularVelocity = PathOfInertia / Radius;

	//get the centrifugal force
	FVector CentrifugalForce = AngularVelocity * AngularVelocity.Size() * Radius;

	return CentrifugalForce;
}
