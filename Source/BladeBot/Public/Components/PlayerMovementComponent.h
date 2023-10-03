// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"
#include "Interface/DebugInterface.h"
#include "PlayerMovementComponent.generated.h"

//custom movement modes for the player character
UENUM()
enum ECustomMovementMode
{
	CCM_None UMETA(DisplayName = "None"),
	CCM_Grappling UMETA(DisplayName = "Grappling")
};

/**
 * Movement component for the player character that adds grappling
 */
UCLASS()
class BLADEBOT_API UPlayerMovementComponent : public UCharacterMovementComponent, public IDebugInterface
{
	GENERATED_BODY()

public:
	UPlayerMovementComponent();

	//reference variables

	//reference to the ownercharacter as a playercharacter
	UPROPERTY(BlueprintReadOnly)
	class APlayerCharacter* PlayerCharacter = nullptr;

	//reference to the grappling hook head
	UPROPERTY(BlueprintReadOnly)
	class AGrapplingHookHead* GrapplingHookRef = nullptr;

	//whether or not to apply a max distance to the grapple hook
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	bool bUseMaxDistance = true;

	//the max distance the grapple hook can reach
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook", meta = (EditCondition = "bUseMaxDistance", editconditionHides))
	float GrappleMaxDistance = 3000.f;

	//the strenght of the pull of the grapple hook
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float PullStrenght = 5000.f;

	//whether or not we should skew the player's velocity when grappling to make it feel more natural
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	bool SkewVelocity = true;

	//the speed at which we skew the player's velocity when grappling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float SkewVelocitySpeed = 10.f;

	//the object that the character is grappling towards
	IGrappleRopeInterface* GrappleObject = nullptr;

	//the current grapple hit used for grappling movement
	FHitResult GrappleHit;

	//override functions
	virtual void BeginPlay() override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;

	//grappling functions

	UFUNCTION(BlueprintCallable)
	void StartGrapple(AActor* GrappleRope);

	UFUNCTION(BlueprintCallable)
	void StopGrapple();

	//function that detects if the player can grapple or not
	bool CanGrapple();

	//line trace function for the grapple hook
	void GrappleLineTrace(FHitResult& OutHit);

	//sets the velocity of the player character when grappling
	void SetGrappleVelocity(float DeltaTime);

	//physics update function for the grappling movement mode
	void PhysGrappling(float DeltaTime, int32 Iterations);

	//get centrifugal force
	FVector GetCentrifugalForce(FVector InVelocity, FVector InGrappleLocation, FVector InGrappleHitLocation);

	//bool checking if the player is dashing
	bool bIsDashing = false;

	//float checking time dashing
	UPROPERTY(EditAnywhere, Category = "DashAttack")
	float DashTime = 5.f;

	//dash deceleration
	UPROPERTY(EditAnywhere, Category = "DashAttack")
	float DashBreaking = 10.f;
	float TempDashBreaking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	float DashFriction = 100.f;
	float TempDashFriction;

	//Timehandler
	FTimerHandle DashTimeHandler;
	void DashCheck();
};