// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/MovementComponent.h"
#include "GrapplingRopeActor.generated.h"

//maybe add a rope tension force to the player when grappling to make it feel more like a real grapple and force the character to move either towards the grapple point or in a swing arc around it

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

UCLASS()
class AGrapplingRopeActor : public AActor, public IGrappleRopeInterface
{
	GENERATED_BODY()
	
public:

	//constructor
	AGrapplingRopeActor();

	//whether or not to use debug drawing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Debug")
	bool bUseDebugDrawing = false;

	//the radius of the rope
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rope")
	float RopeRadius = 10.f;

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

	//array of niagara components used to render the rope
	UPROPERTY(BlueprintReadOnly, Category = "Rope|Rendering")
	TArray<UNiagaraComponent*> NiagaraComponents;

	//the minimum spacing between new and old rope points in the infinite length rope mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rope|InfiniteLength")
	float MinCollisionPointSpacing = 20.f;

	//array of rope points used when the rope is infinite length
	UPROPERTY(BlueprintReadOnly, Category = "Rope|InfiniteLength")
	TArray<FVector> RopePoints;

	//whether or not to use a socket on the instigator pawn to attach the rope to
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rope|Sockets|Instigator")
	bool UseSocketOnInstigator = true;

	//the socket name to use when attaching the rope to the instigator pawn (uses the first mesh with a socket name with the given name)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Sockets|Instigator", meta = (EditCondition = "UseSocketOnInstigator == true", EditConditionHides))
	FName InstigatorSocketName = "HookSlot";

	//reference to the socket mesh to use when attaching the rope to the instigator pawn
	UPROPERTY(BlueprintReadOnly, Category= "Rope|Sockets|Instigator", meta = (EditCondition = "UseSocketOnInstigator == true", EditConditionHides))
	UMeshComponent* InstigatorMesh = nullptr;

	//whether or not to use the jitter on the niagara ribbons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering|Jitter")
	bool bUseJitter = true;

	//the name of the user parameter for the jitter on the niagara ribbons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering|Jitter", meta = (EditCondition = "bUseJitter == true", EditConditionHides))
	FName JitterParameterName = "DoJitter";

	//the tick behaviour to use for the niagara components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering")
	TEnumAsByte<ENiagaraTickBehavior> TickBehavior = ENiagaraTickBehavior::UseComponentTickGroup;

	//the tick group to use for the niagara components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope|Rendering")
	TEnumAsByte<ETickingGroup> TickGroup = ETickingGroup::TG_LastDemotable;

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

	//spawns a new niagara system for a rope point at the given index in the rope points array, pointing towards the next point in the array (not called for the last point in the array)
	void SpawnNiagaraSystem(int Index);

	//renders the rope using the niagara system
	void RenderRope();

	//function to get the length of the rope
	UFUNCTION(BlueprintCallable, Category = "Rope")
	float GetRopeLength() const;

	//called when the owner of this rope is destroyed
	UFUNCTION()
	void OnOwnerDestroyed(AActor* DestroyedActor);
};
