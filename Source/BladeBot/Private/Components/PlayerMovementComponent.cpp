#include "Components/PlayerMovementComponent.h"

#include "Characters/PlayerCharacter.h"
#include "GrapplingHook/GrapplingHookHead.h"
#include "HUD/PlayerOverlay.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	//initialize the grappling hook head
	//GrapplingHookRef = CreateDefaultSubobject<AGrapplingHookHead>(TEXT("GrapplingHookRef"));
}

void UPlayerMovementComponent::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//get the player controller
	PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());

	//get the player character
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	//get the player overlay
	PlayerOverlay = PlayerCharacter->GetPlayerOverlay();
}

void UPlayerMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	//call the parent implementation
	Super::PhysCustom(deltaTime, Iterations);

	//call the appropriate physics function depending on the custom movement mode
	switch (CustomMovementMode)
	{
		case CCM_Grappling:
			PhysGrappling(deltaTime, Iterations);
			break;
		default: ;
	}
}

void UPlayerMovementComponent::PerformMovement(float deltaTime)
{
	//if the player is grappling, don't do anything
	if (CustomMovementMode == CCM_Grappling) return;

	//call the parent implementation
	Super::PerformMovement(deltaTime);
}

void UPlayerMovementComponent::StartGrapple()
{
	//set the movement mode to custom and set the custom movement mode to grappling
	MovementMode = MOVE_Custom;
	CustomMovementMode = CCM_Grappling;
}

void UPlayerMovementComponent::DetectIfCanGrapple()
{
	//if the player overlay is invalid, don't do anything
	if (PlayerOverlay == nullptr) return;

	//do a line trace to see if the player is aiming at something within grapple range
	FHitResult OutHit;
	GrappleLineTrace(OutHit);

	//if the line trace hit something, enable the grappling crosshair and set the in grapple range bool to true
	if (OutHit.bBlockingHit)
	{
		PlayerOverlay->EnableGrapplingCrosshair(true);
		InGrappleRange = true;
	}
	//else disable the grappling crosshair and set the in grapple range bool to false
	else
	{
		PlayerOverlay->EnableGrapplingCrosshair(false);
		InGrappleRange = false;
	}
}

void UPlayerMovementComponent::GrappleLineTrace(FHitResult& OutHit)
{
	//get the camera location and rotation
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	//get the forward vector of the camera rotation
	FVector Rotation = CameraRotation.Quaternion().GetForwardVector();

	//get the end point of the line trace
	FVector End = CameraLocation + Rotation * GrappleMaxDistance;

	//set the collision params
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	//do the line trace
	GetWorld()->LineTraceSingleByChannel(OutHit, CameraLocation, End, ECC_Visibility, CollisionParams);
}

void UPlayerMovementComponent::PhysGrappling(float deltaTime, int32 Iterations)
{
	//if the grappling hook is retracted, don't do anything
	if (IsRetracted) return;

	//if the player is trying to reel in the grappling hook, pull the player towards the grappling hook
	if (TryingTooReel)
	{
		//get the vector between the player and the grappling hook
		FVector VectorBetween = GrapplingHookRef->GetActorLocation() - GetOwner()->GetActorLocation();

		//normalize the vector
		VectorBetween.Normalize();

		//add the vector to the player's velocity
		Velocity += VectorBetween * PullStrenght * deltaTime;
	}
}
