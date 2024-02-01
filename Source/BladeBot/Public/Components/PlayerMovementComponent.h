// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncRootMovement.h"
#include "CollisionQueryParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"
#include "CollisionShape.h"
#include "Math/InterpShorthand.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PlayerMovementComponent.generated.h"

class UPlayerCameraComponent;
class AGrapplingHookHead;

//enum for different grappling modes
UENUM(BlueprintType)
enum EGrapplingMode
{
	AddToVelocity,
	InterpVelocity,
};

/**
 * Movement component for the player character that adds grappling
 */
UCLASS(Blueprintable)
class BLADEBOT_API UPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	//constructor
	UPlayerMovementComponent();

	//declare events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNormalJump);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDirectionalJump, FVector, Direction);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCorrectedDirectionalJump, FVector, OriginalDirection, FVector, CorrectedDirection);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGrapple);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallLatch, FHitResult, HitResult);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallLatchLaunch);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallLatchFall);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallRunStart, FHitResult, HitResult);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallRunJump);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallRunFinish);

	//delegate for the events
	FOnNormalJump OnNormalJump;
	FOnDirectionalJump OnDirectionalJump;
	FOnCorrectedDirectionalJump OnCorrectedDirectionalJump;
	FOnStartGrapple OnStartGrapple;
	FOnWallLatch OnWallLatch;
	FOnWallLatchLaunch OnWallLatchLaunch;
	FOnWallLatchFall OnWallLatchFall;
	FOnWallRunStart OnWallRunStart;
	FOnWallRunJump OnWallRunJump;
	FOnWallRunFinish OnWallRunFinish;

	//whether or not to set the velocity of the player when grappling
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	TEnumAsByte<EGrapplingMode> GrappleMode = InterpVelocity;

	//the grappling speed in add velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float AddGrappleSpeed = 6000.f;

	//the grappling speed in interp velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float InterpGrappleSpeed = 4000.f;

	//the interp function to use when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	TEnumAsByte<EInterpToTargetType> GrappleInterpType = InterpTo;

	//whether or not to use the flying movement mode when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	bool bUseFlyingMovementMode = true;

	//whether to apply a speed boost when stopping grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	bool bEndGrappleSpeedBoost = true;

	//whether to apply the boost in the direction the player is looking or in the direction the player is moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "bEndGrappleSpeedBoost == true", editconditionHides))
	bool bEndGrappleBoostInLookDirection = false;

	//the amount of boost to apply when stopping grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "bEndGrappleSpeedBoost == true", editconditionHides))
	FAsyncRootMovementParams EndGrappleBoostMovementParams = FAsyncRootMovementParams(FVector::UpVector);

	//the interpolation speed when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float GrappleInterpSpeed = 1.5f;

	//whether or not the player is grappling
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	bool bIsGrappling = false;

	//storage for the dot product of the character's velocity and the velocity that was added from grappling last frame
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	float GrappleDotProduct = 0.f;

	//the max acceleration to use when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|Movement")
	float GrappleMaxAcceleration = 8000.f;

	//the float curve to use when applying the grapple velocity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|Movement")
	UCurveFloat* GrappleVelocityCurve = nullptr;

	//the float curve to use when applying the grapple wasd movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|Movement")
	UCurveFloat* GrappleMovementInputCurve = nullptr;

	//the movement input modifier to use when processing the grapple movement input curve
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|Movement")
	float GrappleMovementInputModifier = 1.f;

	//the max speed to use when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|Movement")
	float GrappleMaxSpeed = 4000.f;

	//the max distance the Grappling hook can travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance")
	float MaxGrappleDistance = 9000.f;

	//the max distance to check for when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance")
	float MaxGrappleCheckDistance = 18000.f;

	//the trace channel to use when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance");
	TEnumAsByte<ECollisionChannel> CanGrappleTraceChannel = ECC_Visibility;

	//the amount of force to apply in the direction the player is looking when jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|DirectionalBoost")
	float DirectionalJumpForce = 3000.f;

	//the amount of boost to give to the character while a directional jump is providing force
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|DirectionalBoost")
	float DirectionalJumpGlideForce = 500.f;

	//the amount of boost to apply when boosting a jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|DirectionalBoost")
	float JumpBoostAmount = 500.f;

	//the minimum speed the player needs to be moving at to get a speed based jump boost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|DirectionalBoost")
	float MinSpeedForSpeedBoost = 1000.f;

	//the speed of rotation when using the Rotation mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rotation")
	float RotationSpeed = 1.f;

	//whether or not we can activate rotation mode right now
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Rotation")
	bool bCanActivateRotationMode = true;

	//whether or not WASD effects the player's movement or the player's rotation
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bRotationMode = false;

	//the amount of time to hang before starting to slide down a wall latching wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Latch")
	float WallLatchHangTime = 0.5f;

	//wall latch fall movement params
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Latch")
	FAsyncRootMovementParams WallLatchFallMovementParams = FAsyncRootMovementParams();

	//wall latch launch movement params
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Latch")
	FAsyncRootMovementParams WallLatchLaunchMovementParams = FAsyncRootMovementParams(FVector::DownVector);

	//the tag that activates wall latching when the player is touching an actor with this tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Latch")
	FName WallLatchTag = FName(TEXT("Latch"));

	//the radius of the sphere to use when checking for wall latching walls
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Latch")
	float WallLatchCheckRadius = 50.f;

	//the cooldown between wall latches (needed to prevent wall latching from overriding the launch)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Latch")
	float WallLatchCooldown = 0.5f;

	//whether or not the player is wall latching
	UPROPERTY(BlueprintReadOnly, Category = "Wall Latch")
	bool bIsWallLatching = false;

	//the wall latch hit result
	UPROPERTY(BlueprintReadOnly, Category = "Wall Latch")
	FHitResult WallLatchHitResult;

	//how far to the right or left of the player to check for wall running walls
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Running")
	float WallRunningCheckDistance = 70;

	//how much force to apply to the player to make them stick to the wall when wall running
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Running")
	float WallRunningAttachForce = 1000.f;

	//the tag that activates wall latching when the wall running trace hits an actor with this tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Running")
	FName WallRunningTag = FName(TEXT("Run"));

	//the movement params to use when wall run jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Running")
	FAsyncRootMovementParams WallRunJumpMovementParams = FAsyncRootMovementParams(FVector::UpVector);

	//the cooldown between wall jumps (needed to prevent wall running from overriding wall jumping)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Running")
	float WallJumpCooldown = 0.5f;

	//the current hit result(if any) from the wall running trace
	UPROPERTY(BlueprintReadOnly, Category = "Wall Running")
	FHitResult WallRunningHitResult;

	//whether or not we're is wall running
	UPROPERTY(BlueprintReadOnly, Category = "Wall Running")
	bool bIsWallRunning = false;

	//whether or not we should remain vertical right now (excluding rotation mode == true)
	UPROPERTY()
	bool bShouldRemainVertical = false;

	//reference to the player camera of the player character
	UPROPERTY()
	UPlayerCameraComponent* PlayerCamera = nullptr;

	//the rope that the character is grappling with
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	AGrapplingRopeActor* GrappleRope = nullptr;

	//the collision shape to use when checking if the player can grapple to where they are aiming
	ECollisionShape::Type CanGrappleCollisionShape = ECollisionShape::Sphere;

	//vector pointing in the direction of the grapple
	FVector GrappleDirection = FVector::ZeroVector;

	//whether or not last jump was a directional jump
	bool bLastJumpWasDirectional = false;

	//the direction of the last directional jump
	FVector LastDirectionalJumpDirection = FVector::UpVector;

	//timer handle for the wall latch timer
	FTimerHandle WallLatchFallTimerHandle;

	//the time that the player started a wall jump
	float LastWallJumpTime = 0;

	//the last time the player launched out of a wall latch
	float LastWallLatchLaunchTime = 0;



	//override functions
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const override;
	virtual void Launch(FVector const& LaunchVel) override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual FVector ConsumeInputVector() override;
	virtual bool ShouldRemainVertical() const override;
	virtual bool IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const override;
	virtual float GetGravityZ() const override;

	virtual float GetMinAnalogSpeed() const override;
	virtual bool IsExceedingMaxSpeed(float MaxSpeed) const override;
	virtual float GetMaxSpeed() const override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual float GetMaxAcceleration() const override;
	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;
	virtual void PhysFlying(float deltaTime, int32 Iterations) override;
	virtual void ApplyAccumulatedForces(float DeltaSeconds) override;

	//function that starts the grapple
	UFUNCTION(BlueprintCallable)
	void StartGrapple(AGrapplingRopeActor* InGrappleRope);

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

	//function for boosting jumps
	UFUNCTION(BlueprintCallable)
	void BoostJump(float JumpZVel);

	//function for toggling whether or not wasd effects the player's movement or the player's rotation
	UFUNCTION(BlueprintCallable)
	void ToggleRotationMode(bool InValue = false);

	//function to stop wall latching
	UFUNCTION(BlueprintCallable)
	void StopWallLatch();

	//function to launch off wall latching walls
	UFUNCTION(BlueprintCallable)
	void LaunchOffWallLatch();

	//function to handle wall latching
	UFUNCTION(BlueprintCallable)
	bool DoWallLatch(float DeltaTime);
	void CheckForWallRunFinish();

	//function that updates and returns whether or not the player is wall running
	UFUNCTION(BlueprintCallable)
	bool DoWallRunning(float DeltaTime);

	//function to do a wall running jump
	UFUNCTION(BlueprintCallable)
	void DoWallRunJump(FHitResult InWallHit);

	//function to be used to override canjump internal in the character class
	bool CanJumpAnyway() const;

	////function that's called when we wall jump to prevent the character from jumping again
	//UFUNCTION()
	//void OnWallJumpComplete();
};
