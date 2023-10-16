#include "Components/CustomProjectileMovementComponent.h"

UCustomProjectileMovementComponent::UCustomProjectileMovementComponent()
{
	//bDebugMode = true;
}

void UCustomProjectileMovementComponent::BeginPlay()
{
	//if bSetDistanceCheckLocationOnSpawn is true, set the distance check location to the projectile's location
	if (bSetDistanceCheckLocationOnSpawn)
	{
		SetDistanceCheckLocation(GetOwner()->GetActorLocation());
	}

	//call the parent implementation
	Super::BeginPlay();
}

void UCustomProjectileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//if we're not supposed to tick, return
	if (!bDoTick)
	{
		return;
	}

	//call the parent implementation
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if we're using a max distance, check if we've reached the max distance
	if (bUseMaxDistance)
	{
		CheckMaxDistance();
	}
}

FVector UCustomProjectileMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const
{
	//if we can't move, return zero
	if (!bCanMove)
	{
		return FVector::ZeroVector;
	}

	//if the speed is fixed, return the initial velocity
	//if (bFixedSpeed)
	//{
	//	return InitialVelocity * InitialSpeed / InitialVelocity.Size();
	//}

	//otherwise, return the parent implementation
	return Super::ComputeVelocity(InitialVelocity, DeltaTime);
}

void UCustomProjectileMovementComponent::SetDistanceCheckLocation(const FVector& NewLocation)
{
	//set the distance check location
	DistanceCheckLocation = NewLocation;
}

void UCustomProjectileMovementComponent::CheckMaxDistance()
{
	//if we've reached the max distance
	if (FVector::Dist(DistanceCheckLocation, GetOwner()->GetActorLocation()) >= MaxDistance)
	{
		//call the OnMaxDistReached delegate
		OnMaxDistReached.ExecuteIfBound();

		//print debug message
		DebugPrint(this, -1, 5.f, FColor::Red, "Max distance reached");
	}
}
