#pragma once

#include "CoreMinimal.h"
#include "Characters/StateControl.h"
#include "GameFramework/Actor.h"
#include "GrapplingHookHead.generated.h"

UCLASS()
class BLADEBOT_API AGrapplingHookHead : public AActor
{
	GENERATED_BODY()
	
public:

	//constructor
	AGrapplingHookHead();

	//the rope actor class to spawn with this grappling hook head
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Rope")
	TSubclassOf<class AGrapplingRopeActor> RopeActorClass;


	//whether or not to use a max distance for the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaxDistance")
	bool bUseMaxDistance = false;

	//whether or not to set the distance check location to the projectile's location when the projectile is spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaxDistance", meta = (EditCondition = "bUseMaxDistance == true", editconditionHides))
	bool bUseSpawnForDistanceCheck = true;

	//the max distance the projectile can travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaxDistance", meta = (EditCondition = "bUseMaxDistance == true", editconditionHides))
	float MaxDistance = 10000.f;

	//the location to use when checking if we've reached the max distance
	UPROPERTY(BlueprintReadOnly, Category = "MaxDistance")
	FVector DistanceCheckLocation;

	//the hitbox for the grappling hook head
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Hitbox;

	//the mesh for the grappling hook head
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	//the projectile movement component for the grappling hook head
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	//the current state of the grappling hook head
	UPROPERTY(BlueprintReadOnly)
	EGrappleState GrappleState = EGrappleState::EGS_InAir;

	//reference to the instigator's movement component
	UPROPERTY()
	class UPlayerMovementComponent* PlayerMovementComponent;

	//the rope actor that this grappling hook head is attached to
	UPROPERTY()
	class AGrapplingRopeActor* RopeActor;

	//the initial velocity of the grappling hook head
	FVector InitialVelocity;

	//overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//the function that's called when the grappling hook head overlaps something
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	////despawns/deactivates the grappling hook head (doesn't work)
	//UFUNCTION(BlueprintCallable)
	//void Despawn();

	////reactivates the grappling hook head (doesn't work)
	//UFUNCTION(BlueprintCallable)
	//void Reactivate(FVector NewVelocity);

	//get the current state of the grappling hook head
	FORCEINLINE EGrappleState GetGrappleState() const { return GrappleState; }

	//set the current state of the grappling hook head
	void SetGrappleState(EGrappleState NewState);
};
