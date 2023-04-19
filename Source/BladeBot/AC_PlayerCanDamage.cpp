// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_PlayerCanDamage.h"

// Sets default values for this component's properties
UAC_PlayerCanDamage::UAC_PlayerCanDamage()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_PlayerCanDamage::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAC_PlayerCanDamage::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UAC_PlayerCanDamage::ApplyDamage(int Amount) {
	if (IsDead) return false;
	Health -= Amount;
	if (Health <= 0) {
		// Kill the enemy
		OnEnemyKilled();
		IsDead = true;
		return true;
	}
	OnEnemyDamaged(Amount);
	return false;
}

bool UAC_PlayerCanDamage::OnPlayerBounce_Implementation(AActor* Player, float Speed)
{
	int Damage = 0;
	if (TakesDamageFromBounce) {
		Damage += 1;
	}
	return ApplyDamage(Damage);
}

bool UAC_PlayerCanDamage::OnPlayerAttack_Implementation(AActor* Player, float Speed)
{
	int Damage = DamageFromSlashAttack;
	return ApplyDamage(Damage);
}

bool UAC_PlayerCanDamage::OnPlayerStab_Implementation(AActor* Player, float Speed)
{
	int Damage = DamageFromStabAttack;
	if (StabBonusDamageFromSpeed) {
		Damage += (int)(Speed / SpeedPerExtraStabDamage);
	}
	return ApplyDamage(Damage);
}