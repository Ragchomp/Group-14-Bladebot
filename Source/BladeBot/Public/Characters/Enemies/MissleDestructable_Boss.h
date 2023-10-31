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

	bool CheckSight();
	// line trace
	// line trace fracture
	// repath

	void LineTrace(FVector Target, FHitResult& OutHit);

	// Combat ----------
	void StartBombTimer();
	void BombTimerFinished();
	
	// ------------- class Refs ------------
	UPROPERTY(VisibleAnywhere)
		class AActor* CombatTarget;

	// ------------- Components ------------

	// ------------- Timer Handlers ------------

	FTimerHandle MissleExplosionTimer;




public:
	UPROPERTY(VisibleAnywhere, Category = "Constants")
		float MovementSpeed = 800.f;

	UPROPERTY(VisibleAnywhere, Category = "Constants")
		float RotationSpeed = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Constants")
		float StartExplosionTimerRange = 40.0f;

	UPROPERTY(VisibleAnywhere, Category = "Constants")
		float ExplosionRange = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = "Constants")
		float Damage = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "Constants")
		float MissleExplosionRate = 2.f;

	UPROPERTY(VisibleDefaultsOnly, Category = "Constants")
		bool startedToExplode = false;
};