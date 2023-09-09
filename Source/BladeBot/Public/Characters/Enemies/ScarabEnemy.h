
#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/StateControl.h"
#include "ScarabEnemy.generated.h"

UCLASS()
class BLADEBOT_API AScarabEnemy : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AScarabEnemy();
	virtual void Tick(float DeltaTime) override;

	// ------------ States ---------------------
	EEnemyState EnemyState = EEnemyState::EES_Idle;
	ESGunState GunState = ESGunState::ESGS_Idle;

protected:
	virtual void BeginPlay() override;

	// ------------- Functions ------------

	// Movements ----------
	void CheckIfAtTargetLocation();
	bool InTargetRange(FVector& MovementLocation);
	FVector UpdateRandomTargetPosition();
	void MoveToTargetPosition();

	// Combat ----------
	void SeenAnEnemy();
	void SetTarget();
	void LaserChargeUpComplete();
	void LaserCoolDownComplete();
	void SphereTrace(FHitResult& OutHit);
	void EnemyLeft();

	// Other ----------
	
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
				UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ControllerInit();


private:

	// ------------- class Refs ------------
	UPROPERTY()
		class AAIController* EnemyController;

	UPROPERTY(VisibleAnywhere)
		class AActor* CombatTarget;

	// ------------- Components ------------
	UPROPERTY(VisibleAnywhere)
		class USphereComponent* DetectionSphere;

	// ------------- Timer Handlers ------------

	FTimerHandle MoveToNewLocation;

	FTimerHandle LaserSetTargetTimer;

	FTimerHandle LaserChargeUpTimer;

	FTimerHandle LaserCoolDownTimer;

	// ------------- Constants ------------

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float LaserMaxRange = 3000.f;

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float LaserRadius = 30.f;

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float ChargupUntilSetTargetTimer = 2.f;

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float ChargupAfterTargetSetTimer = 1.f;

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float CooldownTimer = 5.f;

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float WaitAtLocaionMin = 2.f;

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float WaitAtLocaionMax = 5.f;

	UPROPERTY(VisibleInstanceOnly)
	FVector MovementTarget = FVector(0,0,0);

	UPROPERTY()
	FVector LaserTargetPosition = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float MovementRange = 3000.f;

	float AcceptanceRange = 20.f;

	// ------------- Bools ------------

	bool NavWorked = false;

public:
	// ------------- Getters and Setters ------------

		// Gets the AI state from the state controller
	FORCEINLINE EEnemyState GetAIState() const { return EnemyState; }
	FORCEINLINE ESGunState GetScarabGunState() const { return GunState; }

};


