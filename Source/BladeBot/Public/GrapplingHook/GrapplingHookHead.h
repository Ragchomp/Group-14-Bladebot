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

	//whether or not to add the player's speed to the grappling hook head's initial speed when it's spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bAddPlayerSpeed = true;

	//whether or not to add the player's velocity to the grappling hook head's velocity when it's spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bAddPlayerVelocity = false;

	//whether or not to use a max distance for the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaxTravelDistance")
	bool bUseMaxDistance = true;

	//whether or not to set the distance check location to the projectile's location when the projectile is spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaxTravelDistance", meta = (EditCondition = "bUseMaxDistance == true", editconditionHides))
	bool bUseSpawnForDistanceCheck = true;

	//the max distance the projectile can travel
	UPROPERTY(BlueprintReadOnly, Category = "MaxTravelDistance")
	float MaxDistance = 3000.f;

	//the location to use when checking if we've reached the max distance
	UPROPERTY(BlueprintReadOnly, Category = "MaxTravelDistance")
	FVector DistanceCheckLocation;

	//the minimum time the hook must be alive before it can be despawned by player overlap
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Despawn")
	float MinTimeAlive = 0.01f;

	//whether or not the hook can be despawned by player overlap
	UPROPERTY(BlueprintReadOnly, Category = "Despawn")
	bool bCanDespawnbyOverlap = true;

	//whether or not to destroy on impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Despawn")
	bool bDestroyOnImpact = false;

	//the amount of time to wait before destroying the hook after impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Despawn", meta = (EditCondition = "bDestroyOnImpact == true", editconditionHides))
	float DestroyDelay = 0.1f;

	//the hitbox for the grappling hook head's collisions with surfaces
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* WallHitbox;

	//the hitbox for the grappling hook head's collisions with the player
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* PlayerHitbox;

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

	//timer handle for destroying the grappling hook head after a delay
	FTimerHandle DestroyTimer;

	//overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//the function that's called when the grappling hook head overlaps something
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//the function that's called when the grappling hook head stops overlapping something
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//the function that's called when the grappling hook head hits something
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//the function that's called when the grappling hook head is destroyed because of a hit
	UFUNCTION()
	void DoDestroy();

	UFUNCTION()
	void DoDestroy2(AActor* DestroyedActor = nullptr);

	//handles a collision with a wall
	UFUNCTION()
	void HandleWallCollision(const FHitResult& Hit);

	//whether or not the grappling hook has hit a wall
	UFUNCTION(BlueprintCallable)
	bool HasHitWall() const;

	//get the current state of the grappling hook head
	FORCEINLINE EGrappleState GetGrappleState() const { return GrappleState; }
};
