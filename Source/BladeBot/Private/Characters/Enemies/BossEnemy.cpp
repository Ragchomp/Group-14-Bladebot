// Fill out your copyright notice in the Description page of Project Settings.



// Classes
#include "Characters/Enemies/BossEnemy.h"
#include "Characters/PlayerCharacter.h"
#include "Characters/Enemies/MissleDestructable_Boss.h"
#include "Characters/Enemies/MissleIndestructable_Boss.h"

// Components
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABossEnemy::ABossEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// When making a new Mine BP remember to set the gravity scale constant to 0

	// Capsule Init
	GetCapsuleComponent()->SetCapsuleHalfHeight(1000);
	GetCapsuleComponent()->SetCapsuleRadius(600);

	// Mesh Init
	GetMesh()->SetRelativeLocation(FVector(0.f, 30.f, -990.f));
	GetMesh()->SetRelativeScale3D(FVector(6.f, 6.f, 6.f));
	GetMesh()->SetupAttachment(GetRootComponent());

	// Detection sphere init
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetSphereRadius(DetectionRange);
	DetectionSphere->SetupAttachment(GetRootComponent());

	// Detection sphere Collisions
	DetectionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DetectionSphere->SetGenerateOverlapEvents(true);
	DetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	DetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);

	// Initialize the component pointer.
	AttributeComponent = FindComponentByClass<UAttributeComponent>();

}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	EnemyState = EEnemyState::EES_Patroling;

	Tags.Add(FName("Enemy"));

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::OnOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ABossEnemy::EndOverlap);

	// Setts max health and current health to 3 at the start of the game.
	if (AttributeComponent)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, TEXT("In attrib comp"));
		AttributeComponent->SetMaxHealth(MaxHealthOverride);
		AttributeComponent->SetCurrentHealth(CurrentHealthOverride);
	}

	JumpTarget = chooseJumpTarget();

}

float ABossEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	
	if(tokdamageOnce == jumpthreshold)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("tok the damage"));
		// Knockback effect on player if we should have it
		tokdamageOnce++;
		nextPhaseState(PhaseState);
		JumpAway();
		return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	return 0;
	
}

AActor* ABossEnemy::chooseJumpTarget()
{
	//Choose random patrol point
	if (RandomPatrolPointSelection == true)
	{
		Targets.Empty();
		for (AActor* Target : JumpToPositions)
		{
			if (Target != JumpTarget)
			{
				Targets.AddUnique(Target);
			}
		}

		const int32 NumTargets = Targets.Num();
		if (NumTargets > 0)
		{
			const int32 TargetSelection = FMath::RandRange(0, NumTargets - 1);
			return Targets[TargetSelection];
		}
	}
	else // Choose next patrol point in array
	{
		const int32 NumPatrolTargets = JumpToPositions.Num();
		if (NumPatrolTargets > 0)
		{
			CurrentTargetIndex = (CurrentTargetIndex + 1) % NumPatrolTargets;
			return JumpToPositions[CurrentTargetIndex];
		}
	}

	return nullptr;
}

void ABossEnemy::JumpAway()
{
	JumpTarget = chooseJumpTarget();
	SetActorLocation(JumpTarget->GetActorLocation());

	if(tokdamageOnce > jumpthreshold)
	{
		jumpthreshold++;
	}
}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PhaseState == EBossState::E_PhaseZero)
		GEngine->AddOnScreenDebugMessage(4, 1.f, FColor::Green, TEXT("PhaseZero"));
	else if (PhaseState == EBossState::E_PhaseOne)
		GEngine->AddOnScreenDebugMessage(4, 1.f, FColor::Green, TEXT("PhaseOne"));
	else if (PhaseState == EBossState::E_PhaseTwo)
		GEngine->AddOnScreenDebugMessage(4, 1.f, FColor::Green, TEXT("PhaseTwo"));
	else if (PhaseState == EBossState::E_PhaseThree)
		GEngine->AddOnScreenDebugMessage(4, 1.f, FColor::Green, TEXT("PhaseThree"));

}

// ------------ Health ----------------


// handle damage calls jump
// disable handle damage until landed at new location

// ------------ Movement ----------------

// jumping logic to move to next zone

// ------------ Combat ----------------

void ABossEnemy::SeenAnEnemy()
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("enemy Seen"));
	GunState = ESGunState::ESGS_Chargeing;
	PlayVFXChargeUp(GetActorLocation());
	PlayAudioChargeUp(GetActorLocation());
	GetWorldTimerManager().SetTimer(RocketBarrageStartupTimer, this, &ABossEnemy::ShootRocketBarrage, RocketBarrageStartupTime);
}

void ABossEnemy::ShootRocketBarrage()
{
	if (RocketsShoot < MisslesToShoot)
	{
		GunState = ESGunState::ESGS_Shooting;
		//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green, TEXT("Started barrage"));

		//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Spanwned rocket start timer"));
		GetWorldTimerManager().SetTimer(RocketBarrageNextRocketTimer, this, &ABossEnemy::SpawnRocket, MissleIntervalTime);
		RocketsShoot++;
	}
	else
	{
		RocketsShoot = 0;
		//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, TEXT("Started cooldown"));
		GunState = ESGunState::ESGS_Cooling;
		JumpAway();
		GetWorldTimerManager().SetTimer(RocketBarrageCooldownTimer, this, &ABossEnemy::RocketBarrageCooldown, RocketBarrageCooldownTime);
	}

}

FVector ABossEnemy::MissleSpawnLocation()
{
	FVector SpawnLocation = GetActorLocation();
	FVector SpawnLocationOffSet(FMath::RandRange(MissleSpanwLocationoffSettMinX, MissleSpanwLocationoffSettMaxX),
		FMath::RandRange(MissleSpanwLocationoffSettMinY, MissleSpanwLocationoffSettMaxY),
		FMath::RandRange(MissleSpanwLocationoffSettMinZ, MissleSpanwLocationoffSettMaxZ));
	FVector SpawnLocationBaseOffSet = FVector(500.f,500.f,500.f);
	SpawnLocation = SpawnLocation + SpawnLocationOffSet + SpawnLocationBaseOffSet;

	return SpawnLocation;
}

FRotator ABossEnemy::MissleSpawnRotation()
{
	FRotator SpawnRotation = GetActorRotation();
	FRotator SpawnRotationOffSet(FMath::RandRange(MissleSpanwRotatinoffSettMinPitch, MissleSpanwRotatinoffSettMaxPitch),
									FMath::RandRange(MissleSpanwRotatinoffSettMinYaw, MissleSpanwRotatinoffSettMaxYaw), 
									FMath::RandRange(MissleSpanwRotatinoffSettMinRoll, MissleSpanwRotatinoffSettMaxRoll));
	SpawnRotation = SpawnRotation + SpawnRotationOffSet;

	return SpawnRotation;
}

bool ABossEnemy::missleIsIndestructable()
{
	float prob = FMath::RandRange(1, 100);
	
	if (PhaseState == EBossState::E_PhaseZero)
		return false;

	if (PhaseState == EBossState::E_PhaseOne)
		if (prob < (IndestructableMissleProbabiliryPercent / (1 - IndestructableMissleProbabiliryPhaseMod))) return true;

	if (PhaseState == EBossState::E_PhaseTwo)
		if (prob < (IndestructableMissleProbabiliryPercent / (1 - (IndestructableMissleProbabiliryPhaseMod*2)))) return true;

	if (PhaseState == EBossState::E_PhaseThree)
		if (prob < (IndestructableMissleProbabiliryPercent / (1 - (IndestructableMissleProbabiliryPhaseMod * 3)))) return true;

	
	return false;
}

float ABossEnemy::calculateMissleMovementSpeedMod()
{
	float mod = 1.f;

	if (PhaseState == EBossState::E_PhaseZero)
		mod *= 1.f;

	else if (PhaseState == EBossState::E_PhaseOne)
		mod *= 6.2f;

	else if (PhaseState == EBossState::E_PhaseTwo)
		mod *= 10.3f;

	else if (PhaseState == EBossState::E_PhaseThree)
		mod *= 15.5f;

	return mod;
}

void ABossEnemy::SpawnRocket()
{
	//VFX and Audio
	PlayVFXAttack(GetActorLocation());
	PlayAudioAttack(GetActorLocation());
	//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Shot a rocket"));

	// Spawn rocket
	if(MissleDestuctable_BP && MissleIndestuctable_BP)
	{
	
		if(missleIsIndestructable())
		{
			AMissleIndestructable_Boss* NewDoomMissle = GetWorld()->SpawnActor<AMissleIndestructable_Boss>(MissleIndestuctable_BP, MissleSpawnLocation(), MissleSpawnRotation());
			if (NewDoomMissle && CombatTarget)
			{
				NewDoomMissle->SetCombatTarget(CombatTarget);
				NewDoomMissle->updateMovementSpeedMod(calculateMissleMovementSpeedMod());
			}
		}
		else
		{
			AMissleDestructable_Boss* NewMissle = GetWorld()->SpawnActor<AMissleDestructable_Boss>(MissleDestuctable_BP, MissleSpawnLocation(), MissleSpawnRotation());
			if (NewMissle && CombatTarget)
			{
				NewMissle->SetCombatTarget(CombatTarget);
				NewMissle->updateMovementSpeedMod(calculateMissleMovementSpeedMod());
			}
		}

		ShootRocketBarrage();
	}
}

void ABossEnemy::RocketBarrageCooldown()
{
	PlayVFXCoolDown(GetActorLocation());
	PlayAudioCoolDown(GetActorLocation());
	GunState = ESGunState::ESGS_Shooting;
	GetWorldTimerManager().SetTimer(RocketBarrageStartupTimer, this, &ABossEnemy::ShootRocketBarrage, RocketBarrageStartupTime);
}

void ABossEnemy::nextPhaseState(EBossState currentPhase)
{
	if (currentPhase == EBossState::E_PhaseZero)
		PhaseState = EBossState::E_PhaseOne;
	else if (currentPhase == EBossState::E_PhaseOne)
		PhaseState = EBossState::E_PhaseTwo;
	else if (currentPhase == EBossState::E_PhaseTwo)
		PhaseState = EBossState::E_PhaseThree;
	
}

// ------------ Misc ----------------

void ABossEnemy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (CombatTarget == nullptr && Player && Player->ActorHasTag(FName("Player")))
	{
		CombatTarget = Player;
		SeenAnEnemy();
	}
}
