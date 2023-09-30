
#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseEnemy.h"
#include "Characters/StateControl.h"
#include "ScarabEnemy.generated.h"

UCLASS()
class BLADEBOT_API AScarabEnemy : public ABaseEnemy
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

	// Health ----------
	virtual void Die() override;

	// Movements ----------
	void CheckIfAtTargetLocation();
	FVector UpdateRandomTargetPosition();
	void MoveToTargetPosition();

	// Combat ----------
public:
	UFUNCTION(BlueprintCallable)
	void SeenAnEnemy();
	UFUNCTION(BlueprintCallable)
	void SetTarget();
	UFUNCTION(BlueprintCallable)
	void LaserChargeUpComplete();
	UFUNCTION(BlueprintCallable)
	void LaserCoolDownComplete();
	UFUNCTION(BlueprintCallable)
	void EnemyLeft();
	void SphereTrace(FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation, const float& traceRadius);

protected:

	// Other ----------
	
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
				UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

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

	// ------------- Bools ------------

	bool NavWorked = false;

public:

	// ------------- Constants ------------

	UPROPERTY(EditAnywhere, Category = "Constants")
		float LaserMaxRange = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Constants")
		float LaserRadius = 30.f;

	UPROPERTY(EditAnywhere, Category = "Constants")
		float ChargupUntilSetTargetTimer = 2.f;

	UPROPERTY(EditAnywhere, Category = "Constants")
		float ChargupAfterTargetSetTimer = 1.f;

	UPROPERTY(EditAnywhere, Category = "Constants")
		float CooldownTimer = 5.f;

	UPROPERTY(EditAnywhere, Category = "Constants")
		float WaitAtLocaionMin = 2.f;

	UPROPERTY(EditAnywhere, Category = "Constants")
		float WaitAtLocaionMax = 5.f;

	UPROPERTY(VisibleInstanceOnly)
		FVector MovementTarget = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite)
		FVector LaserTargetPosition = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, Category = "Constants")
		float MovementRange = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Constants")
		float Damage = 1.f;

	float AcceptanceRange = 20.f;

	// ------------- Getters and Setters ------------

		// Gets the AI state from the state controller
	FORCEINLINE EEnemyState GetAIState() const { return EnemyState; }
	FORCEINLINE ESGunState GetScarabGunState() const { return GunState; }

};


