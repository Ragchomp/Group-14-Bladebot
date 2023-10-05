// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/StateControl.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"
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
class BLADEBOT_API UPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPlayerMovementComponent();

	//reference variables

	//the strenght of the pull of the grapple hook
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float PullStrenght = 20.f;

	//reference to the ownercharacter as a playercharacter
	UPROPERTY(BlueprintReadOnly)
	class APlayerCharacter* PlayerCharacter = nullptr;

	//the object that the character is grappling towards
	IGrappleRopeInterface* GrappleObject = nullptr;

	//the current grapple state of the grappling object
	EGrappleState GrappleState = EGrappleState::EGS_Retracted;

	//the current grapple hit used for grappling movement
	FHitResult GrappleHit;

	//whether or not the player is grappling
	bool bIsGrappling = false;

	//override functions
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//grappling functions

	UFUNCTION(BlueprintCallable)
	void StartGrapple(AGrapplingRopeActor* GrappleRope);

	UFUNCTION(BlueprintCallable)
	void StopGrapple();

	//function that detects if the player can grapple or not
	bool CanGrapple(float MaxDistance = 3000);

	//line trace function for the grapple hook
	void GrappleLineTrace(FHitResult& OutHit, float MaxDistance);

	//sets the velocity of the player character when grappling
	void SetGrappleVelocity(float DeltaTime);
};
