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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AGrapplingRopeActor> RopeActorClass;

	//the default distance away from the instigator to spawn the grappling hook head
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnDistance = 100.f;

	//the hitbox for the grappling hook head
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Hitbox;

	//the mesh for the grappling hook head
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	//the projectile movement component for the grappling hook head
	UPROPERTY(VisibleAnywhere)
	class UCustomProjectileMovementComponent* ProjectileMovementComponent;

	//the current state of the grappling hook head
	UPROPERTY(BlueprintReadOnly)
	EGrappleState GrappleState = EGrappleState::EGS_InAir;

	//the rope actor that this grappling hook head is attached to
	UPROPERTY(BlueprintReadOnly)
	class AGrapplingRopeActor* RopeActor;

	//the initial velocity of the grappling hook head
	FVector InitialVelocity;

	//reference to the instigator's movement component
	UPROPERTY(BlueprintReadOnly)
	class UPlayerMovementComponent* PlayerMovementComponent;

	//overrides
	virtual void BeginPlay() override;

	//the function that's called when the grappling hook head overlaps something
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//despawns/deactivates the grappling hook head
	UFUNCTION(BlueprintCallable)
	void Despawn();

	//reactivates the grappling hook head
	UFUNCTION(BlueprintCallable)
	void Reactivate(FVector NewVelocity);

	//get the current state of the grappling hook head
	FORCEINLINE EGrappleState GetGrappleState() const { return GrappleState; }
};
