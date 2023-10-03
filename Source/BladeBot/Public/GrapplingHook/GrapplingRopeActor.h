// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Engine/Classes/Components/SphereComponent.h"
#include "Interface/DebugInterface.h"
#include "GrapplingRopeActor.generated.h"


//interface for actors that can be grappled to
UINTERFACE(MinimalAPI, Blueprintable)
class UGrappleRopeInterface : public UInterface
{
	GENERATED_BODY()
};

class IGrappleRopeInterface
{
	GENERATED_BODY()

public:
	virtual FVector GetGrapplePoint(AActor* TravelingActor) const{return FVector::ZeroVector;}
	virtual FVector GetRopeEnd() const{return FVector::ZeroVector;}
};

//enum for the rope mode
UENUM(BlueprintType)
enum ERopeMode
{
	//rope is Infinite
	InfiniteRopeLength UMETA(DisplayName = "InfiniteLength"),

	//rope is a set length
	SetRopeLength UMETA(DisplayName = "SetLength"),

};

//maybe make this a base class for niagara particle system for rope
UCLASS()
class AGrapplingRopeActor : public AActor, public IGrappleRopeInterface, public IDebugInterface
{
	GENERATED_BODY()
	
public:

	//constructor
	AGrapplingRopeActor();

	//whether we should draw debug lines for the rope
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebugRope = false;

	//whether we should draw the physics constraints in the fixed length rope mode (only works if bDrawDebugRope is true)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug|FixedLength", meta = (EditCondition = "bDrawDebugRope", EditConditionHides))
	bool bDrawPhysicsConstraints = false;

	//the radius of the rope
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rope")
	float RopeRadius = 10.f;

	//the rope mode used to determine how the rope behaves
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope")
	TEnumAsByte<ERopeMode> RopeMode = InfiniteRopeLength;

	//the extra space between hitboxes spawned when constructing the physics constraints in the fixed length rope mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rope|FixedLength")
	float HitboxSpacing = 25.f;

	//the minimum spacing between new and old collision points in the infinite length rope mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rope|InfiniteLength")
	float MinCollisionPointSpacing = 20.f;

	//whether or not to use a socket on the instigator pawn to attach the rope to
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sockets|Instigator")
	bool UseSocketOnInstigator = false;

	//the socket name to use when attaching the rope to the instigator pawn (uses the first mesh with a socket name with the given name)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sockets|Instigator", meta = (EditCondition = "UseSocketOnInstigator", EditConditionHides))
	FName InstigatorSocketName;

	//array of collision points used when the rope is infinite length
	UPROPERTY(BlueprintReadOnly, Category = "Rope|InfiniteLength")
	TArray<FVector> CollisionPoints;

	//the Sphere collisions that are used when in the fixed length rope mode
	UPROPERTY(BlueprintReadOnly, category = "Rope|FixedLength")
	TArray<USphereComponent*> Hitboxes;

	//the physics constraints that are used when the in the fixed length rope mode
	UPROPERTY(BlueprintReadOnly, category = "Rope|FixedLength")
	TArray<UPhysicsConstraintComponent*> PhysicsConstraints;

	//reference to the socket mesh to use when attaching the rope to the instigator pawn
	UPROPERTY(BlueprintReadWrite, meta = (EditCondition = "UseSocketOnInstigator", EditConditionHides))
	UStaticMeshComponent* InstigatorMesh = nullptr;

	//whether or not we should tick
	bool bDoTick = true;

	//actor overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//grapple rope interface overrides
	virtual FVector GetGrapplePoint(AActor* TravelingActor) const override;

	//traces along the collision points and removes unnecessary collision points
	void CheckCollisionPoints();

	//sets the collision points for the rope to the current location of the actors we're attached to
	void SetAttachedRopePointPositions(bool FixedLength = false);

	//restarts the rope
	void Restart();

	//draws debug lines for the rope
	void DrawDebugRope();

	//sets the rope mode
	UFUNCTION(BlueprintCallable)
	void SetRopeMode(ERopeMode NewRopeMode);

	//called when the owner of this rope is destroyed
	UFUNCTION()
	void OnOwnerDestroyed(AActor* DestroyedActor);
};
