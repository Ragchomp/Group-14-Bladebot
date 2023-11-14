
// Classes
#include "Characters/Enemies/MissleIndestructable_Boss.h"
#include "Characters/Enemies/BossEnemy.h"

// Components
#include "Kismet/GameplayStatics.h"


AMissleIndestructable_Boss::AMissleIndestructable_Boss()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMissleIndestructable_Boss::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Enemy"));


	if (randomMovementSpeeds == true)
	{
		SetRandomMovementSpeed();
		SetRandomRotationSpeed();
	}
}

void AMissleIndestructable_Boss::SetCombatTarget(AActor* CombatTargetInn)
{
	CombatTarget = CombatTargetInn;
}

void AMissleIndestructable_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//float playerspeed = CombatTarget->GetVelocity().Length();
	//GEngine->AddOnScreenDebugMessage(10, 1, FColor::Orange, FString::Printf(TEXT("%f"), playerspeed));

	Rotate(CombatTarget->GetActorLocation(), DeltaTime);
	Move(DeltaTime);
	StartBombTimer();
}

void AMissleIndestructable_Boss::Rotate(FVector Target, float DeltaTime)
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

void AMissleIndestructable_Boss::Move(float DeltaTime)
{
	FVector NewLocation = GetActorLocation();
	NewLocation += GetActorForwardVector() * MovementSpeed * DeltaTime;
	SetActorLocation(NewLocation);
}

void AMissleIndestructable_Boss::SetRandomMovementSpeed()
{
	MovementSpeed = FMath::RandRange(MovementSpeedMin, MovementSpeedMax);
}

void AMissleIndestructable_Boss::SetRandomRotationSpeed()
{
	RotationSpeed = FMath::RandRange(RotationSpeedMin, RotationSpeedMax);
}

void AMissleIndestructable_Boss::StartBombTimer()
{
	if (CombatTarget && InTargetRange(CombatTarget, StartExplosionTimerRange) && startedToExplode == false)
	{
		PlayVFXChargeUp(GetActorLocation());
		PlayAudioChargeUp(GetActorLocation());
		GetWorldTimerManager().SetTimer(MissleExplosionTimer, this, &AMissleIndestructable_Boss::BombTimerFinished, MissleExplosionRate);
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Orange, FString::Printf(TEXT("StartExploding")));
		startedToExplode = true;
	}
}

void AMissleIndestructable_Boss::BombTimerFinished()
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
