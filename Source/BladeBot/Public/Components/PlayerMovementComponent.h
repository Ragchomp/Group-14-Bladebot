// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"
#include "CollisionShape.h"
#include "Math/InterpShorthand.h"
#include "PlayerMovementComponent.generated.h"

class AGrapplingHookHead;

//maybe add a rope tension force to the player when grappling to make it feel more like a real grapple and force the character to move either towards the grapple point or in a swing arc around it
//maybe add a force in the direction the player is looking at when boosting a jump?

//enum for different grappling modes
UENUM(BlueprintType)
enum EGrapplingMode
{
	SetVelocity,
	AddToVelocity,
	InterpVelocity,
};

//enum for different jump types
UENUM(BlueprintType)
enum EJumpType
{
	Normal,
	AlwaysBoosted,
	CanJumpOffAnySurface, //doesn't work
	BunnyHop,
	BoostedWhenAtLedgeAndMovingTowardsLedge, //draw debug sphere doesn't work
	BoostedWhenMovingFast,
};

//enum for different jump boost types
UENUM(BlueprintType)
enum EJumpBoostType
{
	NoBoost,
	AddToZ,
	SetZ,
	DirectionalJump,
	DirectionalJumpNoBoost,
	DirectionalJumpNoZ,
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float SetGrappleSpeed = 2000.f;

	//the grappling speed in add velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float AddGrappleSpeed = 4000.f;

	//the grappling speed in interp velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float InterpGrappleSpeed = 2000.f;

	//the interp function to use when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	TEnumAsByte<EInterpToTargetType> GrappleInterpType = InterpTo;

	//the interpolation speed when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float GrappleInterpSpeed = 1.5f;

	//whether or not the player is grappling
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	bool bIsGrappling = false;

	//the input modifier to apply to movement input when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|Movement")
	float GrappleMovementInputModifier = 1.f;

	//the max distance the Grappling hook can travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance")
	float MaxGrappleDistance = 4500.f;

	//the max distance to check for when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance")
	float MaxGrappleCheckDistance = 6000.f;

	//the trace channel to use when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance");
	TEnumAsByte<ECollisionChannel> CanGrappleTraceChannel = ECC_Visibility;

	//the current jump type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	TEnumAsByte<EJumpType> JumpType = Normal;

	//the current jump boost type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	TEnumAsByte<EJumpBoostType> JumpBoostType = SetZ;

	//the amount of force to apply in the direction the player is looking when jumping with the DirectionalJump boost type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float DirectionalJumpForce = 1000.f;

	//the max distance away from something the player can be to jump off of it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float JumpOffDistance = 2000.f;

	//the force the player gets away from the wall when wall jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float WallJumpForce = 1000.f;

	//the amount of force to apply in the direction the player is looking when wall jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float WallJumpDirectionalForce = 1000.f;

	//the amount of boost to apply when boosting a jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float JumpBoostAmount = 1000.f;

	//the max movement speed to have when bunny hopping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float MaxBunnyHopSpeed = 1000.f;

	//the minimum movement speed to have when attempting a bunny hopping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float MinBunnyHopSpeed = 500.f;

	//the amount of jump z val to use when bunny hopping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float BunnyHopJumpZVal = 200.f;

	//the amount of time on the ground the character can spend before they can no longer bunny hop
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float BunnyHopTime = 0.5f;

	//the max distance away from a ledge the player can be to jump off of it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float LedgeJumpDistance = 200.f;

	//whether or not to draw a debug sphere at the ledge jump location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	bool bDrawDebugSphere = true;

	//the minimum speed the player needs to be moving at to get a speed based jump boost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	float MinSpeedForSpeedBoost = 1000.f;

	//the collision shape to use when checking if the player can grapple to where they are aiming
	ECollisionShape::Type CanGrappleCollisionShape = ECollisionShape::Sphere;

	//the object that the character is grappling towards
	IGrappleRopeInterface* GrappleObject = nullptr;

	//vector pointing in the direction of the grapple
	FVector GrappleDirection = FVector::ZeroVector;

	//the last hit that the character had
	FHitResult LastHit;

	//whether or not the player is jumping off of a wall
	mutable bool bIsWallJumping = false;

	//whether or not the player is bunny hopping
	mutable bool bIsBunnyHopping = false;

	//the timer handle for the bunny hop timer
	FTimerHandle BunnyHopTimer = FTimerHandle();

	//override functions
	virtual bool CanAttemptJump() const override;
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual bool IsExceedingMaxSpeed(float MaxSpeed) const override;
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;
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

	//function for wall jumping
	UFUNCTION(BlueprintCallable)
	bool WallJump();

	//function for stopping bunny hopping
	UFUNCTION(BlueprintCallable)
	void StopBunnyHop() const;

	//function for boosting jumps
	UFUNCTION(BlueprintCallable)
	void BoostJump(float JumpZVel);
};
