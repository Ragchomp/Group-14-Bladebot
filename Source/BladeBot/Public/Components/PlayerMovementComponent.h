// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"
#include "CollisionShape.h"
#include "Math/InterpShorthand.h"
#include "PlayerMovementComponent.generated.h"

class UPlayerCameraComponent;
class AGrapplingHookHead;

//maybe add a rope tension force to the player when grappling to make it feel more like a real grapple and force the character to move either towards the grapple point or in a swing arc around it
//FVector::HeadingAngle()
//FVector::MirrorByVector()
//FVector::SlerpVectorToDirection()
//FVector::VectorPlaneProject()


//enum for different grappling modes
UENUM(BlueprintType)
enum EGrapplingMode
{
	SetVelocity,
	AddToVelocity,
	InterpVelocity,
};

//enum for different jump boost types
UENUM(BlueprintType)
enum EJumpBoostType
{
	NoBoost,
	AddToZ,
	SetZ,
	DirectionalJump,
	//DirectionalJumpNoBoost,
	//DirectionalJumpNoZ,
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
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	TEnumAsByte<EGrapplingMode> GrappleMode = SetVelocity;

	//the grappling speed in set velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float SetGrappleSpeed = 2000.f;

	//the grappling speed in add velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float AddGrappleSpeed = 4000.f;

	//the grappling speed in interp velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float InterpGrappleSpeed = 4000.f;

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

	//the launch force to apply when starting to grapple
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|Launch")
	float GrappleLaunchForce = 500.f;

	//the max distance the Grappling hook can travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance")
	float MaxGrappleDistance = 4500.f;

	//the max distance to check for when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance")
	float MaxGrappleCheckDistance = 6000.f;

	//the trace channel to use when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance");
	TEnumAsByte<ECollisionChannel> CanGrappleTraceChannel = ECC_Visibility;

	//the current jump boost type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	TEnumAsByte<EJumpBoostType> JumpBoostType = DirectionalJump;

	//the amount of force to apply in the direction the player is looking when jumping with the DirectionalJump boost type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|DirectionalBoost")
	float DirectionalJumpForce = 3000.f;

	//whether or not to boost the jump in the direction the player is looking when velocity is above the MinSpeedForSpeedBoost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|DirectionalBoost")
	bool bBoostJumpInDirectionLooking = true;

	//the amount of boost to apply when boosting a jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|DirectionalBoost")
	float JumpBoostAmount = 500.f;

	//the minimum speed the player needs to be moving at to get a speed based jump boost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|DirectionalBoost")
	float MinSpeedForSpeedBoost = 1000.f;

	//whether or not the player can ever wall jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|Walljump")
	bool bCanEverWallJump = true;

	//the time the player has to jump off of a wall after hitting it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|Walljump")
	float WallJumpTime = 0.1f;

	//the force the player gets away from the wall when wall jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|Walljump")
	float WallJumpForce = 1000.f;

	//the force multipliers to apply to wall jumps (x = impactnormal, y = forwards, z = crossproduct of impactnormal and forwards)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|Walljump")
	FVector WallJumpForceMultipliers = FVector(1.f, 1.f, 1.f);

	//whether or not the player can wall jump when looking any direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|Walljump")
	bool bMustFaceWallToWallJump = true;

	//the modifier to check if the player is looking close enough to the wall to wall jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|Walljump")
	float WallJumpDotModifier = 0.5f;

	//whether or not the player can wall jump right now
	UPROPERTY(BlueprintReadOnly, Category = "Jumping|Walljump")
	bool bCanWallJump = false;

	//reference to the player camera of the player character
	UPROPERTY()
	UPlayerCameraComponent* PlayerCamera = nullptr;

	//the collision shape to use when checking if the player can grapple to where they are aiming
	ECollisionShape::Type CanGrappleCollisionShape = ECollisionShape::Sphere;

	//the object that the character is grappling towards
	IGrappleRopeInterface* GrappleObject = nullptr;

	//vector pointing in the direction of the grapple
	FVector GrappleDirection = FVector::ZeroVector;

	//the last hit that the character had
	FHitResult LastHit;

	//the timer handle for the bunny hop timer
	FTimerHandle WalljumpTimerHandle = FTimerHandle();

	//override functions
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool CanAttemptJump() const override;
	//virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual FVector ConsumeInputVector() override;
	virtual bool HandlePendingLaunch() override;

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

	//function to check if the player is looking in a direction that is close enough to the wallhits normal to wall jump
	UFUNCTION(BlueprintCallable)
	bool IsFacingNearWall() const;

	//function for checking if the player can wall jump
	UFUNCTION(BlueprintCallable)
	bool CanWallJump() const;

	//function for wall jumping
	UFUNCTION(BlueprintCallable)
	void DoWallJump();

	//function for setting can wall jumo to false
	UFUNCTION(BlueprintCallable)
	void DisableWallJump();

	//function for boosting jumps
	UFUNCTION(BlueprintCallable)
	void BoostJump(float JumpZVel);
};
