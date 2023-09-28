// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/StateControl.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	//reference to the ownercharacter's playercontroller
	UPROPERTY(BlueprintReadOnly)
	APlayerController* PlayerController = nullptr;

	//reference to the ownercharacter as a playercharacter
	UPROPERTY(BlueprintReadOnly)
	class APlayerCharacter* PlayerCharacter = nullptr;

	//reference to the ownercharacter's playeroverlay
	UPROPERTY(BlueprintReadOnly)
	class UPlayerOverlay* PlayerOverlay = nullptr;

	//reference to the grappling hook head
	UPROPERTY(BlueprintReadOnly)
	class AGrapplingHookHead* GrapplingHookRef = nullptr;

	//grappling variables

	//whether the grapple hook is retracted or not
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GrappleHook|Display")
	bool IsRetracted = true;

	//whether the player is trying to reel in the grapple hook or not
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GrappleHook|Display")
	bool TryingTooReel = false;

	//whether the where the player is aiming is in grapple range or not
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GrappleHook|Display")
	bool InGrappleRange = false;

	//whether the grappling functions are in debug mode or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook|Display")
	bool DebugModeGrappling = false;

	//whether the grapple hook is at max teather or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook|Display")
	bool IsMaxTeather = true;


	//the grapple hook head class used for the grappling hook
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AGrapplingHookHead> BP_GrapplingHookHead;

	//the max distance the grapple hook can reach
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float GrappleMaxDistance = 3000.f;

	//the strenght of the pull of the grapple hook
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float PullStrenght = 5000.f;

	//the current state of the grapple hook
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook|State")
	EGrappleState GrappleHookState = EGrappleState::EGS_Retracted;

	//override functions
	virtual void BeginPlay() override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void PerformMovement(float deltaTime) override;

	//grappling functions

	UFUNCTION(BlueprintCallable)
	void StartGrapple();

	//function that detects if the player can grapple or not
	void DetectIfCanGrapple();

	//line trace function for the grapple hook
	void GrappleLineTrace(FHitResult& OutHit);

	//physics update function for the grappling movement mode
	void PhysGrappling(float deltaTime, int32 Iterations);
};
