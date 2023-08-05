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
	AGrapplingHookHead();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void Despawn();
protected:
	virtual void BeginPlay() override;

	/** Class Components  */
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* Mesh;

	/** Class Functions  */
	void Move(float DeltaTime);


	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);

	/** Class Constants  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float ProjectileSpeed = 4000.f;

	/** Bools  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4-Bools")
		bool CanMove = true;

private:

	/** State Control  */
	EGrappleState GrappleState = EGrappleState::EGS_Retracted;

public:	
	FORCEINLINE EGrappleState GetGrappleState() const { return GrappleState; }
};
