
// Classes
#include "Characters/Enemies/MineEnemy.h"
#include "Characters/PlayerCharacter.h"

// Components
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"

AMineEnemy::AMineEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// When making a new Mine BP remember to set the gravity scale constant to 0

	// Capsule Init
	GetCapsuleComponent()->SetCapsuleHalfHeight(75);
	GetCapsuleComponent()->SetCapsuleRadius(75);

	// Mesh Init
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -30.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 180.f, -30.f));
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
}

void AMineEnemy::BeginPlay()
{
	Super::BeginPlay();
	EnemyState = EEnemyState::EES_Patroling;
	GunState = ESGunState::ESGS_Idle;

	Tags.Add(FName("AIMine"));

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMineEnemy::OnOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AMineEnemy::EndOverlap);

	// Moves to first patrol
	PatrolTarget = ChoosePatrolTarget();
		
}

void AMineEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckPatrolTarget();

	if (CanMove==true)
	{
		MoveToTarget(DeltaTime);
	}
}

// Movement -------------------

void AMineEnemy::Die()
{
	GetWorldTimerManager().ClearTimer(DischargeChargeUpTimer);
	GetWorldTimerManager().ClearTimer(DischargeCoolDownTimer);
	Super::Die();
}

AActor* AMineEnemy::ChoosePatrolTarget()
{

	//Choose random patrol point
	if (RandomPatrolPointSelection == true)
	{
		Targets.Empty();
		for (AActor* Target : PatrolTargets)
		{
			if (Target != PatrolTarget)
			{
				Targets.AddUnique(Target);
			}
		}

		const int32 NumPatrolTargets = Targets.Num();
		if (NumPatrolTargets > 0)
		{
			const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
			return Targets[TargetSelection];
		}
	}
	else // Choose next patrol point in array
	{
		const int32 NumPatrolTargets = PatrolTargets.Num();
		if (NumPatrolTargets > 0)
		{
			CurrentTargetIndex = (CurrentTargetIndex + 1) % NumPatrolTargets;
			return PatrolTargets[CurrentTargetIndex];
		}
	}

	return nullptr;
}

void AMineEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		CanMove = false;
		const float WaitTime = FMath::RandRange(PatrolDelayMin, PatrolDelayMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AMineEnemy::PatrolTimerFinished, WaitTime);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("At point, start timer"));
	}
}

void AMineEnemy::PatrolTimerFinished()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("patroltimer finished"));
	CanMove = true;
}

bool AMineEnemy::RotateToFace(float DeltaTime, FVector Direction)
{

	FRotator StartRoatation = GetActorRotation();
	FRotator EndRoatation = Direction.Rotation();
	float Alpha = 1.f;

	FRotator InterpolatedRotation = FMath::Lerp(StartRoatation, EndRoatation, Alpha);
	SetActorRotation(InterpolatedRotation * DeltaTime);

	if(InterpolatedRotation == EndRoatation)
	{
		return true;
	}

	return false;
}

void AMineEnemy::MoveToTarget(float DeltaTime)
{
	if(PatrolTarget)
	{
		
		FVector Point1 = GetActorLocation();
		FVector Point2 = PatrolTarget->GetActorLocation();
		FVector MovementVector = GetVectorBetweenTwoPoints(Point1, Point2);

		FRotator CurrentRotation = GetActorRotation();
		FRotator TargetRotation = MovementVector.Rotation();

		FRotator NewRotation = FMath::Lerp(CurrentRotation, TargetRotation, RotationSpeed);
		SetActorRotation(NewRotation);

		if(CurrentRotation.Equals(TargetRotation, 5.f))
		{
			FVector NewLocation = GetActorLocation();
			NewLocation += MovementVector * MovementSpeed * DeltaTime;

			SetActorLocation(NewLocation);
		}

	}
}

// Combat -------------------

void AMineEnemy::SeenAnEnemy()
{
	if (EnemyState == EEnemyState::EES_Attacking || CombatTarget == nullptr) return;

	EnemyState = EEnemyState::EES_Attacking;

	// Charge gun
	if (GunState == ESGunState::ESGS_Idle)
	{
		GunState = ESGunState::ESGS_Chargeing;
		PlayVFXChargeUp(GetActorLocation());
		PlayAudioChargeUp(GetActorLocation());
		GetWorldTimerManager().SetTimer(DischargeChargeUpTimer, this, &AMineEnemy::DischargeChargeUpComplete, DischargeChargeRate);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Charging"));
	}
}

void AMineEnemy::DischargeChargeUpComplete()
{
	if (GunState != ESGunState::ESGS_Chargeing) return;

	GunState = ESGunState::ESGS_Shooting;
	if (InTargetRange(CombatTarget, DischargeMaxRange))
	{
		// Player Takes damage here
		PlayVFXAttack(GetActorLocation());
		PlayAudioAttack(GetActorLocation());
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player Take Damage"));
	}

	GunState = ESGunState::ESGS_Cooling;
	PlayVFXCoolDown(GetActorLocation());
	PlayAudioCoolDown(GetActorLocation());
	GetWorldTimerManager().SetTimer(DischargeCoolDownTimer, this, &AMineEnemy::DischargeCoolDownComplete, DischargeCooldownRate);
}

void AMineEnemy::DischargeCoolDownComplete()
{
	if (GunState != ESGunState::ESGS_Cooling) return;

	GunState = ESGunState::ESGS_Idle;
	if (EnemyState == EEnemyState::EES_Attacking)
	{
		GunState = ESGunState::ESGS_Chargeing;
		PlayVFXChargeUp(GetActorLocation());
		PlayAudioChargeUp(GetActorLocation());
		GetWorldTimerManager().SetTimer(DischargeChargeUpTimer, this, &AMineEnemy::DischargeChargeUpComplete, DischargeChargeRate);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Cooldown complete -> charging"));
	}
}

void AMineEnemy::EnemyLeft()
{
	// Clear all combat timers
	GetWorldTimerManager().ClearTimer(DischargeChargeUpTimer);
	GetWorldTimerManager().ClearTimer(DischargeCoolDownTimer);
	// Start cooldown on Discharge
	GunState = ESGunState::ESGS_Cooling;
	GetWorldTimerManager().SetTimer(DischargeCoolDownTimer, this, &AMineEnemy::DischargeCoolDownComplete, DischargeCooldownRate);
	// goes back to patrolling
	CombatTarget = nullptr;
	EnemyState = EEnemyState::EES_Patroling;
}

// Other -------------------

void AMineEnemy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player && Player->ActorHasTag(FName("Player")))
	{
		CombatTarget = Player;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Detected player"));
		SeenAnEnemy();
	}
}

void AMineEnemy::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EnemyLeft();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player gone"));
}
