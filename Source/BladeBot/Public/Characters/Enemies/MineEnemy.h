
#pragma once

#include "CoreMinimal.h"
#include "Characters/StateControl.h"
#include "Characters/BaseEnemy.h"
#include "MineEnemy.generated.h"

UCLASS()
class BLADEBOT_API AMineEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AMineEnemy();
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
	AActor* ChoosePatrolTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();
	bool RotateToFace(float DeltaTime, FVector Direction);
	void MoveToTarget(float DeltaTime);

	// Combat ----------
	void SeenAnEnemy();
	void DischargeChargeUpComplete();
	void DischargeCoolDownComplete();
	void EnemyLeft();

	// Other ----------

	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:

	// ------------- class Refs ------------
	UPROPERTY(VisibleAnywhere)
		class AActor* CombatTarget;

	UPROPERTY(VisibleAnywhere)
		class AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "Patrol")
		TArray<AActor*> PatrolTargets;

	UPROPERTY()
		TArray<AActor*> Targets;
	// ------------- Components ------------
	UPROPERTY(VisibleAnywhere)
		class USphereComponent* DetectionSphere;


	// ------------- Timer Handlers ------------

	FTimerHandle PatrolTimer;

	FTimerHandle DischargeChargeUpTimer;

	FTimerHandle DischargeCoolDownTimer;


public:

	// ------------- Constants ------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		float DischargeMaxRange = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		float DischargeChargeRate = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		float DischargeCooldownRate = 3.f;

	UPROPERTY()
		float DetectionRange = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		double Damage = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		double DamageRange = 2000.f;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		double PatrolRadius = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		float MovementSpeed = 200.f;

	// Timer Delays
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		float PatrolDelayMin = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		float PatrolDelayMax = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constants")
		float RotationSpeed = 0.02f;

	UPROPERTY()
	int32 CurrentTargetIndex = -1;

	// ------------- Bools ------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
	bool CanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
	bool RandomPatrolPointSelection = true;

	// ------------- Getters and Setters ------------

		// Gets the AI state from the state controller
	FORCEINLINE EEnemyState GetAIState() const { return EnemyState; }

};
