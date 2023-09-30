// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/StateControl.h"
#include "Characters/BaseEnemy.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BLADEBOT_API ABossEnemy : public ABaseEnemy
{
	GENERATED_BODY()


public:
	ABossEnemy();
	virtual void Tick(float DeltaTime) override;

	// ------------ States ---------------------
	EEnemyState EnemyState = EEnemyState::EES_Idle;

protected:
	virtual void BeginPlay() override;

	// ------------- Functions ------------

	// Health ----------

	// Movements ----------

	// Combat ----------
	void SeenAnEnemy();
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

	// ------------- Components ------------
	UPROPERTY(VisibleAnywhere)
		class USphereComponent* DetectionSphere;


	// ------------- Timer Handlers ------------



public:

	// ------------- Constants ------------
	UPROPERTY()
	float DetectionRange = 5000.f;

	// Timer Delays


	// ------------- Bools ------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool CanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool RandomPatrolPointSelection = true;

	// ------------- Getters and Setters ------------

		// Gets the AI state from the state controller
	FORCEINLINE EEnemyState GetAIState() const { return EnemyState; }

};
