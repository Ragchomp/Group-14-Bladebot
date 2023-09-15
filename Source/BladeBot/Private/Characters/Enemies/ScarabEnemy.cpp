
// Classes
#include "Characters/Enemies/ScarabEnemy.h"
#include "Characters/PlayerCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"

// Components
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Navigation/PathFollowingComponent.h"

AScarabEnemy::AScarabEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh Init
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -170.f));
	GetMesh()->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));

	// Capsule Init
	GetCapsuleComponent()->SetCapsuleHalfHeight(170);
	GetCapsuleComponent()->SetCapsuleRadius(100);
	RootComponent = GetCapsuleComponent();

	// Detection sphere init
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(GetRootComponent());
	DetectionSphere->SetSphereRadius(LaserMaxRange);

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

void AScarabEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	ControllerInit();

	EnemyState = EEnemyState::EES_Patroling;
	GunState = ESGunState::ESGS_Idle;
	MovementTarget = GetActorLocation();

	Tags.Add(FName("AI"));

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AScarabEnemy::OnOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AScarabEnemy::EndOverlap);
}

void AScarabEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckIfAtTargetLocation();

}

// Movement --------------------

void AScarabEnemy::Die()
{
	GetWorldTimerManager().ClearTimer(LaserSetTargetTimer);
	GetWorldTimerManager().ClearTimer(LaserChargeUpTimer);
	GetWorldTimerManager().ClearTimer(LaserCoolDownTimer);
	Super::Die();
}

void AScarabEnemy::CheckIfAtTargetLocation()
{
	if(InTargetRange(MovementTarget,AcceptanceRange + 300) && EnemyState == EEnemyState::EES_Patroling)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("At Location"));
		MovementTarget = UpdateRandomTargetPosition();
		const float WaitTime = FMath::RandRange(WaitAtLocaionMin, WaitAtLocaionMax);
		GetWorldTimerManager().SetTimer(MoveToNewLocation, this, &AScarabEnemy::MoveToTargetPosition, WaitTime);
	}
}

FVector AScarabEnemy::UpdateRandomTargetPosition()
{
	FVector RandomLocation;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSystem)
	{
		FNavLocation NavLocation;
		FVector Origin = GetActorLocation();
		
		if (NavSystem->GetRandomPointInNavigableRadius(Origin, MovementRange, NavLocation))
			RandomLocation = NavLocation.Location;
		
		else
			UpdateRandomTargetPosition();
		
	}

	return RandomLocation;
}

void AScarabEnemy::MoveToTargetPosition()
{
	if (EnemyState != EEnemyState::EES_Patroling && !EnemyController) return;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Move to target triggereed"));
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(MovementTarget);
	MoveRequest.SetAcceptanceRadius(AcceptanceRange);
	EnemyController->SetFocalPoint(MovementTarget);
	EnemyController->MoveTo(MoveRequest);
}

// Combat --------------------

void AScarabEnemy::SeenAnEnemy()
{
	if (EnemyState == EEnemyState::EES_Attacking || CombatTarget == nullptr) return;

	// Charge gun
	if(GunState == ESGunState::ESGS_Idle)
	{
		GunState = ESGunState::ESGS_Chargeing;
		PlayVFXChargeUp(GetActorLocation());
		PlayAudioChargeUp(GetActorLocation());
		GetWorldTimerManager().SetTimer(LaserSetTargetTimer, this, &AScarabEnemy::SetTarget, ChargupUntilSetTargetTimer);
	}

	// Stop patrolling
	EnemyState = EEnemyState::EES_Attacking;
	GetWorldTimerManager().ClearTimer(MoveToNewLocation);

	// Make Combattarget focus
	EnemyController->SetFocus(CombatTarget);
	
}

void AScarabEnemy::SetTarget()
{
	if(CombatTarget)
	{
		FVector StartLocation = GetActorLocation();
		FVector TargetLocation = CombatTarget->GetActorLocation();
		FVector ShootDirection = (TargetLocation - StartLocation).GetSafeNormal();
		LaserTargetPosition = StartLocation + ShootDirection * LaserMaxRange;
		GetWorldTimerManager().SetTimer(LaserChargeUpTimer, this, &AScarabEnemy::LaserChargeUpComplete, ChargupAfterTargetSetTimer);
	}
}

void AScarabEnemy::LaserChargeUpComplete()
{
	if (GunState != ESGunState::ESGS_Chargeing) return;

	GunState = ESGunState::ESGS_Shooting;
	FHitResult OutHit;
	SphereTrace(OutHit,GetActorLocation(),LaserTargetPosition,LaserRadius);
	PlayVFXAttack(GetActorLocation());
	PlayAudioAttack(GetActorLocation());

	PlayVFXCoolDown(GetActorLocation());
	PlayAudioCoolDown(GetActorLocation());
	GunState = ESGunState::ESGS_Cooling;
	GetWorldTimerManager().SetTimer(LaserCoolDownTimer, this, &AScarabEnemy::LaserCoolDownComplete, CooldownTimer);
}

void AScarabEnemy::LaserCoolDownComplete()
{
	if (GunState != ESGunState::ESGS_Cooling) return;

	GunState = ESGunState::ESGS_Idle;
	if(EnemyState == EEnemyState::EES_Attacking)
	{
		GunState = ESGunState::ESGS_Chargeing;
		PlayVFXChargeUp(GetActorLocation());
		PlayAudioChargeUp(GetActorLocation());
		GetWorldTimerManager().SetTimer(LaserSetTargetTimer, this, &AScarabEnemy::SetTarget, ChargupUntilSetTargetTimer);
	}
}

void AScarabEnemy::EnemyLeft()
{
	// clear all combat timers
	GetWorldTimerManager().ClearTimer(LaserSetTargetTimer);
	GetWorldTimerManager().ClearTimer(LaserChargeUpTimer);
	GetWorldTimerManager().ClearTimer(LaserCoolDownTimer);
	// start cooldown on gun
	GunState = ESGunState::ESGS_Cooling;
	GetWorldTimerManager().SetTimer(LaserCoolDownTimer, this, &AScarabEnemy::LaserCoolDownComplete, CooldownTimer);
	// goes back to patrolling
	EnemyState = EEnemyState::EES_Patroling;
	const float WaitTime = FMath::RandRange(WaitAtLocaionMin, WaitAtLocaionMax);
	GetWorldTimerManager().SetTimer(MoveToNewLocation, this, &AScarabEnemy::MoveToTargetPosition, WaitTime);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("No longer an attacktarget"));
	CombatTarget = nullptr;
}

// Other --------------------

void AScarabEnemy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player && Player->ActorHasTag(FName("Player")))
	{
		CombatTarget = Player;
		SeenAnEnemy();
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Attacktarget"));
	}
}

void AScarabEnemy::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EnemyLeft();
}

void AScarabEnemy::ControllerInit()
{
	EnemyController = Cast<AAIController>(GetController());
}

