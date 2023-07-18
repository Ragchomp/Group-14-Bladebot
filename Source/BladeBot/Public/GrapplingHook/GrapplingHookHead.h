#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrapplingHookHead.generated.h"

UCLASS()
class BLADEBOT_API AGrapplingHookHead : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrapplingHookHead();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Class Functions  */
	void Move(float DeltaTime);
	void Despawn();

	/** Class Constants  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float ProjectileSpeed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float ProjectileLife = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float ProjectileMaxLife = 3.f;


private:

	/** Class Components  */
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	

public:	


};
