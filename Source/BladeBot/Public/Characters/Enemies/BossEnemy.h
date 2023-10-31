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
	ESGunState GunState = ESGunState::ESGS_Idle;

protected:
	virtual void BeginPlay() override;

	// ------------- Functions ------------

	// Health ----------

	// Movement ----------

	// Combat ----------
	void SeenAnEnemy();
	void ShootRocketBarrage();
	// Spawning ----------
	FVector MissleSpawnLocation();
	FRotator MissleSpawnRotation();
	void SpawnRocket();
	// /Spawning ----------
	void RocketBarrageCooldown();

	// Other ----------

	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;

private:

	// ------------- class Refs ------------
	UPROPERTY(VisibleAnywhere)
		class AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Components")
		TSubclassOf<class AMissleDestructable_Boss> MissleDestuctable_BP;

	// ------------- Components ------------
	UPROPERTY(VisibleAnywhere)
		class USphereComponent* DetectionSphere;

	// Declare a pointer to the component in your subclass.
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UAttributeComponent* AttributeComponent;

	// ------------- Timer Handlers ------------

	FTimerHandle RocketBarrageStartupTimer;

	FTimerHandle RocketBarrageNextRocketTimer;

	FTimerHandle RocketBarrageCooldownTimer;

public:

	// ------------- Constants ------------
	UPROPERTY()
		float DetectionRange = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float CurrentHealthOverride = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MaxHealthOverride = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float RocketBarrageStartupTime = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float NextRocketIntervalTime = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float RocketBarrageCooldownTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		int RocketsToShoot = 10;

	UPROPERTY()
		int RocketsShoot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwRotatinoffSettMinPitch = -45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwRotatinoffSettMaxPitch = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwRotatinoffSettMinYaw = -45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwRotatinoffSettMaxYaw = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwRotatinoffSettMinRoll = -45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwRotatinoffSettMaxRoll = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwLocationoffSettMinX = -1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwLocationoffSettMaxX = -1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwLocationoffSettMinY = -1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwLocationoffSettMaxY = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwLocationoffSettMinZ = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constatnts")
		float MissleSpanwLocationoffSettMaxZ = 400.f;
	
	// ------------- Bools ------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool CanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools")
		bool RandomPatrolPointSelection = true;

	// ------------- Getters and Setters ------------

		// Gets the AI state from the state controller
	FORCEINLINE EEnemyState GetAIState() const { return EnemyState; }
	FORCEINLINE ESGunState GetGunState() const { return GunState; }
	FORCEINLINE AActor* GetCombatTarget() const { return CombatTarget; }
};
