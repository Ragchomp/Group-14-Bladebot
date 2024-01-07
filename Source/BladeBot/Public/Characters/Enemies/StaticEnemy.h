
#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseEnemy.h"
#include "Characters/StateControl.h"
#include "StaticEnemy.generated.h"

UCLASS()
class BLADEBOT_API AStaticEnemy : public ABaseEnemy
{
	GENERATED_BODY()
public:
	AStaticEnemy();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// ------------ States ---------------------
	EEnemyState EnemyState = EEnemyState::EES_Idle;
	ESGunState GunState = ESGunState::ESGS_Idle;

	// ------------------ FUNCTIONS ------------------

	// Health ----------
	virtual void Die() override;

	// Offense ----------
	void SeenAnEnemy();
	void EnemyLeft();

	// LASER COMBAT
	UFUNCTION(BlueprintCallable)
		void SetTarget();
	UFUNCTION(BlueprintCallable)
		void LaserChargeUpComplete();
	UFUNCTION(BlueprintCallable)
		void LaserCoolDownComplete();
	void SphereTrace(FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation, const float& traceRadius);

	// Other ----------
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	// ------------------ MEMBERS ------------------

	// ------------- class Refs ------------
	UPROPERTY(VisibleAnywhere)
		class AActor* CombatTarget;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* DetectionSphere;

	UPROPERTY()
		class AAIController* EnemyController;

public:

	// ------------- Timer Handlers ------------

	FTimerHandle MoveToNewLocation;

	FTimerHandle LaserSetTargetTimer;

	FTimerHandle LaserChargeUpTimer;

	FTimerHandle LaserCoolDownTimer;

	// ------------- Constants ------------

	UPROPERTY()
		float DetectionRange = 2000.f;

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

	UPROPERTY(BlueprintReadWrite)
		FVector LaserTargetPosition = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, Category = "Constants")
		float Damage = 1.f;

	// ------------- Bools ------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool CanMove = true;

public:

	// ------------------ GET/SET ------------------

	FORCEINLINE EEnemyState GetAIState() const { return EnemyState; }
	FORCEINLINE ESGunState GetGunState() const { return GunState; }
};
