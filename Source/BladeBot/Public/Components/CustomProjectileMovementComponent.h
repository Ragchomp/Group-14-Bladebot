// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/DebugInterface.h"
#include "CustomProjectileMovementComponent.generated.h"


DECLARE_DELEGATE(FMaxDistDelegate)
DECLARE_DELEGATE(FDespawnDelegate)

/**
 * 
 */
UCLASS()
class BLADEBOT_API UCustomProjectileMovementComponent : public UProjectileMovementComponent, public IDebugInterface
{
	GENERATED_BODY()

public:

	//constructor
	UCustomProjectileMovementComponent();

	//whether or not the projectile's speed is fixed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Movement")
	bool bFixedSpeed = false;

	//whether or not to use a max distance for the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Movement")
	bool bUseMaxDistance = false;

	//whether or not to set the distance check location to the projectile's location when the projectile is spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Movement")
	bool bSetDistanceCheckLocationOnSpawn = false;

	//whether or not we can move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Movement")
	bool bCanMove = true;

	//the max distance the projectile can travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Movement", meta = (EditCondition = "bUseMaxDistance", editconditionHides))
	float MaxDistance = 10000.f;

	//whether or not to tick this component
	bool bDoTick = true;

	//the location to use when checking if we've reached the max distance
	FVector DistanceCheckLocation;

	//delegate for when the projectile reaches the max distance
	FMaxDistDelegate OnMaxDistReached;

	//delegate for when the projectile is despawned
	FDespawnDelegate OnDespawn;

	//overrides
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual FVector ComputeVelocity(FVector InitialVelocity, float DeltaTime) const override;

	//sets the location to use when checking if we've reached the max distance
	void SetDistanceCheckLocation(const FVector& NewLocation);

	//checks if we've reached the max distance
	void CheckMaxDistance();
};
