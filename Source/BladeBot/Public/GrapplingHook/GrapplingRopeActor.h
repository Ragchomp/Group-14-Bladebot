// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Engine/Classes/Components/SphereComponent.h"
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
};

//enum for the rope mode
UENUM(BlueprintType)
enum ERopeMode
{
	//rope is Infinite
	InfiniteRopeLength UMETA(DisplayName = "InfiniteLength"),

	//rope is a set length
	FixedLength UMETA(DisplayName = "FixedLength")
};

UCLASS()
class AGrapplingRopeActor : public AActor, public IGrappleRopeInterface
{
	GENERATED_BODY()
	
public:

	//constructor
	AGrapplingRopeActor();

	//the radius of the rope
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rope")
	float RopeRadius = 10.f;

	//the rope mode used to determine how the rope behaves
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope")
	TEnumAsByte<ERopeMode> RopeMode = InfiniteRopeLength;

	//the Niagara system used to render the rope
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering")
	TObjectPtr<UNiagaraSystem> NiagaraSystem = nullptr;

	//the name of the user parameter for the end of the Niagara ribbons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering")
	FName RibbonEndParameterName = "RopeEnd";

	//whether or not to use the rope radius as the ribbon width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering")
	bool UseRopeRadiusAsRibbonWidth = true;

	//the name of the user parameter for the width of the Niagara ribbons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering", meta = (EditCondition = "UseRopeRadiusAsRibbonWidth == false", EditConditionHides))
	FName RibbonWidthParameterName = "RopeWidth";

	//the width of the ribbon used to render the rope(only used if UseRopeRadiusAsRibbonWidth is false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering", meta = (EditCondition = "UseRopeRadiusAsRibbonWidth == false", EditConditionHides))
	float RibbonWidth = 10.f;

	//whether 

	//whether we should draw debug lines for the rope
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering")
	bool bDrawDebugRope = false;

	//whether we should draw the physics constraints in the fixed length rope mode (only works if bDrawDebugRope is true)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering", meta = (EditCondition = "RopeMode == ERopeMode::FixedLength && bDrawDebugRope == true", EditConditionHides))
	bool bDrawPhysicsConstraints = false;

	//array of niagara components used to render the rope
	UPROPERTY(BlueprintReadOnly, Category = "Rope|Rendering")
	TArray<UNiagaraComponent*> NiagaraComponents;

	//the minimum spacing between new and old rope points in the infinite length rope mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rope|InfiniteLength", meta = (editcondition = "RopeMode == ERopeMode::InfiniteRopeLength", editconditionHides))
	float MinCollisionPointSpacing = 20.f;

	//array of rope points used when the rope is infinite length
	UPROPERTY(BlueprintReadOnly, Category = "Rope|InfiniteLength")
	TArray<FVector> RopePoints;

	//the extra space between hitboxes spawned when constructing the physics constraints in the fixed length rope mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rope|FixedLength", meta = (editcondition = "RopeMode == ERopeMode::FixedLength", editconditionHides))
	float HitboxSpacing = 25.f;

	//the Sphere collisions that are used when in the fixed length rope mode
	UPROPERTY(BlueprintReadOnly, category = "Rope|FixedLength")
	TArray<USphereComponent*> Hitboxes;

	//the physics constraints that are used when the in the fixed length rope mode
	UPROPERTY(BlueprintReadOnly, category = "Rope|FixedLength")
	TArray<UPhysicsConstraintComponent*> PhysicsConstraints;

	//whether or not to use a socket on the instigator pawn to attach the rope to
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope|Sockets|Instigator")
	bool UseSocketOnInstigator = false;

	//the socket name to use when attaching the rope to the instigator pawn (uses the first mesh with a socket name with the given name)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Sockets|Instigator", meta = (EditCondition = "UseSocketOnInstigator == true", EditConditionHides))
	FName InstigatorSocketName;

	//reference to the socket mesh to use when attaching the rope to the instigator pawn
	UPROPERTY(BlueprintReadOnly, Category= "Rope|Sockets|Instigator", meta = (EditCondition = "UseSocketOnInstigator == true", EditConditionHides))
	UMeshComponent* InstigatorMesh = nullptr;

	//whether or not to use the jitter on the niagara ribbons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering|Jitter")
	bool bUseJitter = true;

	//the name of the user parameter for the jitter on the niagara ribbons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering|Jitter", meta = (EditCondition = "bUseJitter == true", EditConditionHides))
	FName JitterParameterName = "DoJitter";

	//actor overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	//grapple rope interface overrides
	virtual FVector GetGrapplePoint(AActor* TravelingActor) const override;

	//traces along the collision points and removes unnecessary collision points
	void CheckCollisionPoints();

	//sets the collision points for the rope to the current location of the actors we're attached to
	void SetAttachedRopePointPositions(bool FixedLength = false);

	//renders the rope using the niagara system
	void RenderRope();

	//draws debug lines for the rope
	void DrawDebugRope();

	//sets the rope mode
	UFUNCTION(BlueprintCallable)
	void SetRopeMode(ERopeMode NewRopeMode);

	//called when the owner of this rope is destroyed
	UFUNCTION()
	void OnOwnerDestroyed(AActor* DestroyedActor);
};
