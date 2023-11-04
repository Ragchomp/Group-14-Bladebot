// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"
#include "CollisionShape.h"
#include "InputActionValue.h"
#include "Math/InterpShorthand.h"
#include "PlayerMovementComponent.generated.h"

class AGrapplingHookHead;

//unsure about implementation of grapple slide, wondering if it would be better to modify friction and braking deceleration instead of isvalidlandingspot
//unsure about how to implement grapple movement input in the setvelocity mode
	//maybe track the player's input and use that to in the set velocity mode?

//enum for different grappling modes
UENUM(BlueprintType)
enum EGrapplingMode
{
	SetVelocity,
	AddToVelocity,
	InterpVelocity,
};

/**
 * Movement component for the player character that adds grappling
 */
UCLASS()
class BLADEBOT_API UPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPlayerMovementComponent();

	//whether or not to set the velocity of the player when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	TEnumAsByte<EGrapplingMode> GrappleMode = SetVelocity;

	//the grappling speed in set velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::SetVelocity", EditConditionHides))
	float SetGrappleSpeed = 2000.f;

	//the grappling speed in add velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::AddToVelocity", EditConditionHides))
	float AddGrappleSpeed = 4000.f;

	//the grappling speed in add velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::InterpToGrapple", EditConditionHides))
	float InterpGrappleSpeed = 2000.f;

	//the movement speed to use when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float SetVelGrappleMoveSpeed = 2000.f;

	//the interp function to use when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::InterpToGrapple", EditConditionHides))
	TEnumAsByte<EInterpToTargetType> GrappleInterpType = InterpTo;

	//the interpolation speed when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::InterpToGrapple", EditConditionHides))
	float GrappleInterpSpeed = 2.5f;

	//whether or not the player is grappling
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	bool bIsGrappling = false;

	//the max distance the Grappling hook can travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float MaxGrappleDistance = 3000.f;

	//the max distance to check for when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|CanGrapple")
	float MaxGrappleCheckDistance = 6000.f;

	//the trace channel to use when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|CanGrapple");
	TEnumAsByte<ECollisionChannel> CanGrappleTraceChannel = ECC_Visibility;

	//the distance to use when checking for floors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloorCheck")
	float FloorCheckDistance = 1000.f;

	//the grappling hook that the player is using
	UPROPERTY(BlueprintReadOnly, Category = "GrapplingHook")
	AGrapplingHookHead* GrapplingHook = nullptr;

	//the input vector to use when grappling
	FVector GrappleInputVector = FVector::ZeroVector;

	//grapple slide timer handle
	FTimerHandle GrappleSlideTimerHandle;

	//the collision shape to use when checking if the player can grapple to where they are aiming
	ECollisionShape::Type CanGrappleCollisionShape = ECollisionShape::Sphere;

	//the object that the character is grappling towards
	IGrappleRopeInterface* GrappleObject = nullptr;

	//vector pointing in the direction of the grapple
	FVector GrappleVelocity = FVector::ZeroVector;

	//override functions
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PhysFalling(float DeltaTime, int32 Iterations) override;
	virtual void PhysFlying(float DeltaTime, int32 Iterations) override;
	virtual FVector ConsumeInputVector() override;

	//function that starts the grapple
	UFUNCTION(BlueprintCallable)
	void StartGrapple(AGrapplingRopeActor* GrappleRope);

	//function that stops the grapple
	UFUNCTION(BlueprintCallable)
	void StopGrapple();

	//function that detects if the player can grapple or not
	UFUNCTION(BlueprintCallable)
	bool CanGrapple() const;

	//gets the distance to the point the player is looking at minus the max grapple distance or returns 0 if that distance is less than 0 or what the player is looking at is not within the max grapple check distance
	UFUNCTION(BlueprintCallable)
	float GetGrappleDistanceLeft() const;

	//line trace function for the grapple hook
	void GrappleLineTrace(FHitResult& OutHit, float MaxDistance) const;

	//sets the velocity of the player character when grappling
	void UpdateGrappleVelocity(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void StopSliding();
};
