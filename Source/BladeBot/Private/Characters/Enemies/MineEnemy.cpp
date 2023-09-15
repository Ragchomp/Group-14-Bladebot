
// Classes
#include "Characters/Enemies/MineEnemy.h"
#include "Characters/PlayerCharacter.h"

// Components
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
//#include "GameFramework/CharacterMovementComponent.h"

AMineEnemy::AMineEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh Init
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -30.f));

	// Capsule Init
	GetCapsuleComponent()->SetCapsuleHalfHeight(75);
	GetCapsuleComponent()->SetCapsuleRadius(75);
	RootComponent = GetCapsuleComponent();

	// Detection sphere init
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(GetRootComponent());
	DetectionSphere->SetSphereRadius(DetectionRange);

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
	// Finds all patrol targets that are valid (not currently at)
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	// Setts a random destination based on the array of valid targets
	const int32 NumPatroltargets = ValidTargets.Num();
	if (NumPatroltargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatroltargets - 1);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Choosen new target"));
		return ValidTargets[TargetSelection];
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

void AMineEnemy::MoveToTarget(float DeltaTime)
{
	if(PatrolTarget)
	{
		FVector Point1 = GetActorLocation();
		FVector Point2 = PatrolTarget->GetActorLocation();
		FVector MovementVector = GetVectorBetweenTwoPoints(Point1, Point2);

		FVector NewLocation = GetActorLocation();
		NewLocation += MovementVector * MovementSpeed * DeltaTime;
		SetActorLocation(NewLocation);
		//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Yellow, TEXT("Moveing"));
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player Take Damage"));
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
