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

UENUM(BlueprintType)
enum EGrapplingMode
{
	SetVelocity,
	AddToVelocity,
	InterpToGrapple,
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
	TEnumAsByte<EGrapplingMode> GrappleMode = InterpToGrapple;

	//the grappling speed in set velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::SetVelocity", EditConditionHides))
	float SetGrappleSpeed = 2000.f;

	//the grappling speed in add velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::AddToVelocity", EditConditionHides))
	float AddGrappleSpeed = 4000.f;

	//the grappling speed in add velocity mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::InterpToGrapple", EditConditionHides))
	float InterpGrappleSpeed = 2000.f;

	//the interp function to use when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::InterpToGrapple", EditConditionHides))
	TEnumAsByte<EInterpToTargetType> GrappleInterpType = InterpTo;

	//the interpolation speed when using the InterpToGrapple mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling", meta = (EditCondition = "GrappleMode == EGrapplingMode::InterpToGrapple", EditConditionHides))
	float GrappleInterpSpeed = 2.5f;

	//whether or not the player is grappling
	UPROPERTY(BlueprintReadOnly, Category = "Grappling")
	bool bIsGrappling = false;

	//the grappling hook that the player is using
	UPROPERTY(BlueprintReadOnly, Category = "Grappling|GrappleHook")
	AGrapplingHookHead* GrapplingHook = nullptr;

	//the max distance the Grappling hook can travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grappling|GrappleHook")
	float MaxGrappleDistance = 3000.f;

	//the trace channel to use when checking if the player can grapple to where they are aiming
	UPROPERTY(EditAnywhere, Category = "Grappling|CanGrapple");
	TEnumAsByte<ECollisionChannel> CanGrappleTraceChannel = ECC_Visibility;

	//the collision shape to use when checking if the player can grapple to where they are aiming
	ECollisionShape::Type CanGrappleCollisionShape = ECollisionShape::Sphere;

	//the object that the character is grappling towards
	IGrappleRopeInterface* GrappleObject = nullptr;

	//vector pointing in the direction of the grapple
	FVector GrappleVelocity = FVector::ZeroVector;

	//override functions
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PhysFlying(float deltaTime, int32 Iterations) override;

	//function that starts the grapple
	UFUNCTION(BlueprintCallable)
	void StartGrapple(AGrapplingRopeActor* GrappleRope);

	//function that stops the grapple
	UFUNCTION(BlueprintCallable)
	void StopGrapple();

	//function that detects if the player can grapple or not
	bool CanGrapple() const;

	//line trace function for the grapple hook
	void GrappleLineTrace(FHitResult& OutHit) const;

	//sets the velocity of the player character when grappling
	void UpdateGrappleVelocity(float DeltaTime);
};
