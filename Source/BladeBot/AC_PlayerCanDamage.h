// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BladeBot/I_DamageEventContainer.h"
#include "AC_PlayerCanDamage.generated.h"

class AActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLADEBOT_API UAC_PlayerCanDamage : public UActorComponent, public II_DamageEventContainer
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_PlayerCanDamage();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Damage Model")
		void OnEnemyDamaged(int Amount);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Damage Model")
		void OnEnemyKilled();

	bool ApplyDamage(int Damage);

	bool IsDead = false;

	FString Name = "Enemy";

	// Number of attacks the enemy can endure
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Model")
		int Health = 5;

	// Damage dealt by a simple slash attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Model")
		int	DamageFromSlashAttack = 1;

	// Does the player deal damage if they bounce into the enemy?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Model")
		bool TakesDamageFromBounce = false;
	
	// Damage dealt by the player when they stab the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Model")
		int	DamageFromStabAttack = 2;

	// Does the enemy take extra stab damage from speed?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Model")
		bool StabBonusDamageFromSpeed = false;
	
	// Speed required for bonus stab damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Model")
		int SpeedPerExtraStabDamage = 10;

	// OnPlayerBounce
	// When the player hits an enemy at speed
	// Returns whether the enemy was killed
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage Events")
		bool OnPlayerBounce_Implementation(AActor* Player, float Speed) override;

	// OnPlayerAttack
	// When the player hits an enemy with an attack
	// Returns whether the enemy was killed
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage Events")
		bool OnPlayerAttack_Implementation(AActor* Player, float Speed) override;

	// OnPlayerStab
	// When the player hits an enemy at speed with a stab
	// Returns whether the enemy was killed
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage Events")
		bool OnPlayerStab_Implementation(AActor* Player, float Speed) override;
};
