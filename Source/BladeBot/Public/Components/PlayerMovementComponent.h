// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"
#include "CollisionShape.h"
#include "Math/InterpShorthand.h"
#include "PlayerMovementComponent.generated.h"

// should override get max speed and get max acceleration to have better control over the player's movement

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

	//delegate for the events
	FOnNormalJump OnNormalJump;
	FOnDirectionalJump OnDirectionalJump;
	FOnCorrectedDirectionalJump OnCorrectedDirectionalJump;
	FOnStartGrapple OnStartGrapple;

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
	float EndGrappleBoostAmount = 1000.f;

	//the interpolation speed when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float GrappleInterpSpeed = 1.5f;

	//whether or not the player is grappling
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	bool bIsGrappling = false;

	//the input modifier to apply to movement input when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|Movement")
	float GrappleMovementInputModifier = 1.f;

	//the max acceleration to use when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|Movement")
	float GrappleMaxAcceleration = 8000.f;

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

	//whether or not WASD effects the player's movement or the player's rotation
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bRotationMode = false;

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

	//override functions
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const override;
	virtual void Launch(FVector const& LaunchVel) override;
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual FVector ConsumeInputVector() override;
	virtual bool ShouldRemainVertical() const override;
	virtual bool IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const override;


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
};
