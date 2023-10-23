// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissleDestructable_Boss.generated.h"

UCLASS()
class BLADEBOT_API AMissleDestructable_Boss : public AActor
{
	GENERATED_BODY()
	
public:	
	AMissleDestructable_Boss();
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void SetCombatTarget(AActor* CombatTargetInn);

protected:
	virtual void BeginPlay() override;

private:

	// ------------- Functions ------------

	// Health -----------
	void Die();

	// Movement ----------
	void Rotate(FVector Target, float DeltaTime);
	void Move(float DeltaTime);

	bool CheckSight();
	// line trace
	// line trace fracture
	// repath

	void LineTrace(FVector Target, FHitResult& OutHit);

	// Combat ----------


	// ------------- class Refs ------------
	UPROPERTY(VisibleAnywhere)
		class AActor* CombatTarget;

	// ------------- Components ------------

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UBoxComponent* Collision;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USkeletalMeshComponent* Mesh;

	// Declare a pointer to the component in your subclass.
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UAttributeComponent* AttributeComponent;

public:
	UPROPERTY(VisibleAnywhere, Category = "Constants")
		float MovementSpeed = 800.f;

	UPROPERTY(VisibleAnywhere, Category = "Constants")
		float RotationSpeed = 1.0f;

	UFUNCTION(BlueprintCallable)
		FVector GetVectorBetweenTwoPoints(const FVector& Point1, const FVector& Point2);

};