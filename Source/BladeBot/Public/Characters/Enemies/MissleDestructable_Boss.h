// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseEnemy.h"
#include "GameFramework/Actor.h"
#include "MissleDestructable_Boss.generated.h"

UCLASS()
class BLADEBOT_API AMissleDestructable_Boss : public ABaseEnemy
{
	GENERATED_BODY()
	
public:	
	AMissleDestructable_Boss();
	virtual void Tick(float DeltaTime) override;

	void SetCombatTarget(AActor* CombatTargetInn);

protected:
	virtual void BeginPlay() override;

private:

	// ------------- Functions ------------

	// Health -----------

	// Movement ----------
	void Rotate(FVector Target, float DeltaTime);
	void Move(float DeltaTime);
	void SetRandomMovementSpeed();
	void SetRandomRotationSpeed();

	//void LineTrace(FVector Target, FHitResult& OutHit);

	// Combat ----------
	void StartBombTimer();
	void BombTimerFinished();
	void instantExplode();
	
	// ------------- class Refs ------------
	UPROPERTY(VisibleAnywhere)
		class AActor* CombatTarget;

	// ------------- Components ------------

	// ------------- Timer Handlers ------------

	FTimerHandle MissleExplosionTimer;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float MovementSpeedMin = 500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float MovementSpeedMax = 1400.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float MovementSpeed = 1000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float RotationSpeedMin = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float RotationSpeedMax = 1.3f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float RotationSpeed = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float StartExplosionTimerRange = 300.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float ExplosionRange = 400.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float Damage = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float MissleExplosionRate = 0.3f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Constants")
		float PlayerSpeedToNotDieFromExplosion = 2500.f;

	UPROPERTY(VisibleDefaultsOnly, Category = "Bools")
		bool startedToExplode = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bools")
		bool randomMovements = true;
};