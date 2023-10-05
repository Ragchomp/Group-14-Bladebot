#include "Components/PlayerMovementComponent.h"

#include "Characters/PlayerCharacter.h"
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

void UPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//call the parent implementation
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//check if the player is grappling
	if (bIsGrappling)
	{
		//set the grapple velocity
		SetGrappleVelocity(DeltaTime);
	}
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
		
		//vlerp the characters velocity to the vector applied by grappling so as to help improve game feel
		//UKismetMathLibrary::VLerp()
	}
}

void UPlayerMovementComponent::StopGrapple()
{
	if (bIsGrappling)
	{
		//set is grappling to false
		bIsGrappling = false;

		//set the grapple object to null
		GrappleObject = nullptr;
	}
}

bool UPlayerMovementComponent::CanGrapple(float MaxDistance)
{
	//do a line trace to see if the player is aiming at something within grapple range
	GrappleLineTrace(GrappleHit, MaxDistance);

	//if the line trace hit something, return true
	if (GrappleHit.bBlockingHit)
	{
		return true;
	}

	return false;
}

void UPlayerMovementComponent::GrappleLineTrace(FHitResult& OutHit, float MaxDistance)
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

	//set the collision params
	FCollisionQueryParams CollisionParams;

	//ignore the owner of the grapple hook
	CollisionParams.AddIgnoredActor(GetOwner());

	//do the line trace
	GetWorld()->LineTraceSingleByChannel(OutHit, CameraLocation, End, ECC_Visibility, CollisionParams);
}

void UPlayerMovementComponent::SetGrappleVelocity(float DeltaTime)
{
	//get the point the character is grappling to
	const FVector GrapplePoint = GrappleObject->GetGrapplePoint(GetCharacterOwner());

	//get the vector from the character to the grapple point
	const FVector GrappleVector = (GrapplePoint - GetCharacterOwner()->GetActorLocation()).GetSafeNormal();

	//apply the grapple vector to the character's velocity
	Velocity = GrappleVector * PullStrenght;

	//update the character's velocity
	UpdateComponentVelocity();
}

