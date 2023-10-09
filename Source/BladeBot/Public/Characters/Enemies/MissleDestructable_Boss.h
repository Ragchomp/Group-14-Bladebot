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

	UFUNCTION(BlueprintCallable)
		virtual void HandleDamage(const float& DamageAmount);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;

private:

	// ------------- Functions ------------

	void Die();

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



};
