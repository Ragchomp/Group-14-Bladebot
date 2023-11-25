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

	//declare the OnNormalJump event
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNormalJump);

	//declare the OnDirectionalJump event
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDirectionalJump);

	//declare the OnCanWallJump event
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCanWallJump);

	//declare the OnWallJump event
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallJump);

	//whether or not to set the velocity of the player when grappling
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	TEnumAsByte<EGrapplingMode> GrappleMode = InterpVelocity;

	//the grappling speed in add velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float AddGrappleSpeed = 4000.f;

	//the grappling speed in interp velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	float InterpGrappleSpeed = 4000.f;

	//the interp function to use when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	TEnumAsByte<EInterpToTargetType> GrappleInterpType = InterpTo;

	//whether or not to use the flying movement mode when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling")
	bool bUseFlyingMovementMode = true;

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
	float MaxGrappleDistance = 9000.f;

	//the max distance to check for when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance")
	float MaxGrappleCheckDistance = 18000.f;

	//the trace channel to use when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|MaxDistance");
	TEnumAsByte<ECollisionChannel> CanGrappleTraceChannel = ECC_Visibility;

	//the amount of force to apply in the direction the player is looking when jumping with the DirectionalJump boost type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|DirectionalBoost")
	float DirectionalJumpForce = 3000.f;

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
	float WallJumpForce = 2000.f;

	//the upwards force the player gets when wall jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|Walljump")
	float WallJumpZVel = 1000.f;

	//whether or not the wall jump force should be based on the player's velocity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|Walljump")
	bool bScaleWallJumpForceByVelocity = true;

	//the amount to scale the wall jump force by when using the ScaleWallJumpForceByVelocity option
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping|Walljump")
	float WallJumpForceVelocityScale = 0.1f;

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

	//the length of the rope when the player started grappling
	float GrappleRopeLength = 0.f;

	//the last hit that the character had
	FHitResult LastHit;

	//the timer handle for the bunny hop timer
	FTimerHandle WalljumpTimerHandle = FTimerHandle();

	//the event for when the player does a normal jump
	FOnNormalJump OnNormalJump;

	//the event for when the player does a directional jump
	FOnDirectionalJump OnDirectionalJump;

	//the event for when the player starts colliding with a wall and can wall jump
	FOnCanWallJump OnCanWallJump;

	//the event for when the player wall jumps
	FOnWallJump OnWallJump;

	//override functions
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Launch(FVector const& LaunchVel) override;
	virtual bool CanAttemptJump() const override;
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual FVector ConsumeInputVector() override;
	//virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;

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

	////blueprint event for when the player starts grappling
	//UFUNCTION(BlueprintImplementableEvent, Category = "Grappling")
	//void OnStartGrapple();

	////blueprint event for when the player stops grappling
	//UFUNCTION(BlueprintImplementableEvent, Category = "Grappling")
	//void OnStopGrapple();

	////blueprint event for when the player does a normal jump
	//UFUNCTION(BlueprintImplementableEvent, Category = "Jumping")
	//void OnNormalJump();

	////blueprint event for when the player does a directional jump
	//UFUNCTION(BlueprintImplementableEvent, Category = "Jumping")
	//void OnDirectionalJump();

	////blueprint event for when the player starts colliding with a wall and can wall jump
	//UFUNCTION(BlueprintImplementableEvent, Category = "Jumping|Walljump")
	//void OnCanWallJump();

	////blueprint event for when the player wall jumps
	//UFUNCTION(BlueprintImplementableEvent, Category = "Jumping|Walljump")
	//void OnWallJump();

};
