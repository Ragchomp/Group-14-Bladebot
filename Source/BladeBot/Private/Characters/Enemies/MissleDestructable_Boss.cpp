
// Classes
#include "Characters/Enemies/MissleDestructable_Boss.h"
#include "Characters/Enemies/BossEnemy.h"

// Components
#include "Kismet/GameplayStatics.h"


AMissleDestructable_Boss::AMissleDestructable_Boss()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMissleDestructable_Boss::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Enemy"));


	if(randomMovements == true)
	{
		SetRandomMovementSpeed();
		SetRandomRotationSpeed();
	}
}

void AMissleDestructable_Boss::SetCombatTarget(AActor* CombatTargetInn)
{
	CombatTarget = CombatTargetInn;
}

void AMissleDestructable_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//float playerspeed = CombatTarget->GetVelocity().Length();
	//GEngine->AddOnScreenDebugMessage(10, 1, FColor::Orange, FString::Printf(TEXT("%f"), playerspeed));

	Rotate(CombatTarget->GetActorLocation(), DeltaTime);
	Move(DeltaTime);
	StartBombTimer();
}

void AMissleDestructable_Boss::Rotate(FVector Target, float DeltaTime)
{
	if (CombatTarget)
	{
		FVector Point1 = GetActorLocation();
		FVector Point2 = Target;
		FVector MovementVector = GetVectorBetweenTwoPoints(Point1, Point2);
		
		FRotator CurrentRotation = GetActorRotation();
		FRotator TargetRotation = MovementVector.Rotation();

		FRotator NewRotation = FMath::Lerp(CurrentRotation, TargetRotation, RotationSpeed);
		SetActorRotation(NewRotation);
	}
}

void AMissleDestructable_Boss::Move(float DeltaTime)
{
	FVector NewLocation = GetActorLocation();
	NewLocation += GetActorForwardVector() * MovementSpeed * DeltaTime;
	SetActorLocation(NewLocation);
}

void AMissleDestructable_Boss::SetRandomMovementSpeed()
{
	MovementSpeed = FMath::RandRange(MovementSpeedMin, MovementSpeedMax);
}

void AMissleDestructable_Boss::SetRandomRotationSpeed()
{
	RotationSpeed = FMath::RandRange(RotationSpeedMin, RotationSpeedMax);
}

void AMissleDestructable_Boss::StartBombTimer()
{
	if(CombatTarget && InTargetRange(CombatTarget, StartExplosionTimerRange) && startedToExplode == false)
	{ 
		PlayVFXChargeUp(GetActorLocation());
		PlayAudioChargeUp(GetActorLocation());
		GetWorldTimerManager().SetTimer(MissleExplosionTimer, this, &AMissleDestructable_Boss::BombTimerFinished, MissleExplosionRate);
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Orange, FString::Printf(TEXT("StartExploding")));
		startedToExplode = true;
	}
}

void AMissleDestructable_Boss::BombTimerFinished()
{
	if (CombatTarget && InTargetRange(CombatTarget, ExplosionRange))
	{
		if (CombatTarget->ActorHasTag("Player") && CombatTarget->GetVelocity().Length() < PlayerSpeedToNotDieFromExplosion)
			UGameplayStatics::ApplyDamage(CombatTarget, Damage, this->GetInstigatorController(), this, UDamageType::StaticClass());
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Orange, FString::Printf(TEXT("Exploded, hit player")));
	}
	PlayVFXAttack(GetActorLocation());
	PlayAudioAttack(GetActorLocation());
	Die();
}

void AMissleDestructable_Boss::updateMovementSpeedMod(float mod)
{
	MovementSpeedMin *= mod;
	MovementSpeedMax *= mod;

}