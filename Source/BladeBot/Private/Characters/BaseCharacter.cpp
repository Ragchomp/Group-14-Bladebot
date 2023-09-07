

#include "Characters/BaseCharacter.h"


//Math Includes
#include "Math/Vector.h"
#include "Math/Rotator.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;



}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ABaseCharacter::Die()
{

}

void ABaseCharacter::LineTrace(FHitResult& OutHit)
{

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{

}

void ABaseCharacter::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

FVector ABaseCharacter::GetPointWithRotator(const FVector& Start, const FRotator& Rotation, float Distance)
{
	// Convert the rotation to a quaternion
	FQuat Quaternion = Rotation.Quaternion();

	// Create a direction vector from the quaternion
	FVector Direction = Quaternion.GetForwardVector();

	// Calculate the coordinates of the point using the direction vector and distance
	FVector Point = Start + (Direction * Distance);

	return Point;
}

FVector ABaseCharacter::GetVectorOfRotation(const FRotator& Rotation)
{
	// Convert the rotation to a quaternion
	FQuat Quaternion = Rotation.Quaternion();

	// Create a direction vector from the quaternion
	FVector Direction = Quaternion.GetForwardVector();

	return Direction;
}

FVector ABaseCharacter::GetVectorBetweenTwoPoints(const FVector& Point1, const FVector& Point2)
{
	FVector VectorBetweenLocations = Point1 - Point2;
	VectorBetweenLocations.Normalize();

	return -VectorBetweenLocations;
}

float ABaseCharacter::GetDistanceBetweenTwoPoints(const FVector& Point1, const FVector& Point2)
{
	float DistanceToTarget = FVector::Dist(Point1, Point2);

	return DistanceToTarget;
}
