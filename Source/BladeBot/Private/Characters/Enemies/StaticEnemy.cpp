// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemies/StaticEnemy.h"

#include "Characters/PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

AStaticEnemy::AStaticEnemy()
{
	// Capsule Init
	GetCapsuleComponent()->SetCapsuleHalfHeight(75);
	GetCapsuleComponent()->SetCapsuleRadius(75);

	// Mesh Init
	GetMesh()->SetRelativeLocation(FVector(0.f, 30.f, -300.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 90, 0.f));
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

void AStaticEnemy::BeginPlay()
{
	Super::BeginPlay();
	EnemyState = EEnemyState::EES_Patroling;
	GunState = ESGunState::ESGS_Idle;

	Tags.Add(FName("Enemy"));

	EnemyController = Cast<AAIController>(GetController());
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AStaticEnemy::OnOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AStaticEnemy::EndOverlap);
}

void AStaticEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	if (CanMove == true)
	{
		//MoveToTarget(DeltaTime);
	}
}

void AStaticEnemy::Die()
{
	GetWorldTimerManager().ClearTimer(LaserSetTargetTimer);
	GetWorldTimerManager().ClearTimer(LaserChargeUpTimer);
	GetWorldTimerManager().ClearTimer(LaserCoolDownTimer);
	Super::Die();
}

void AStaticEnemy::SeenAnEnemy()
{
	if (EnemyState == EEnemyState::EES_Attacking || CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Attacking;
	// Charge gun
	if (GunState == ESGunState::ESGS_Idle)
	{
		GunState = ESGunState::ESGS_Chargeing;
		PlayVFXChargeUp(GetActorLocation());
		PlayAudioChargeUp(GetActorLocation());
		GetWorldTimerManager().SetTimer(LaserSetTargetTimer, this, &AStaticEnemy::SetTarget, ChargupUntilSetTargetTimer);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Setting target"));
	}

	// Make Combattarget focus
	EnemyController->SetFocus(CombatTarget);
}

void AStaticEnemy::EnemyLeft()
{
	// clear all combat timers
	GetWorldTimerManager().ClearTimer(LaserSetTargetTimer);
	GetWorldTimerManager().ClearTimer(LaserChargeUpTimer);
	GetWorldTimerManager().ClearTimer(LaserCoolDownTimer);
	// start cooldown on gun
	GunState = ESGunState::ESGS_Cooling;
	EnemyState = EEnemyState::EES_Idle;
	GetWorldTimerManager().SetTimer(LaserCoolDownTimer, this, &AStaticEnemy::LaserCoolDownComplete, CooldownTimer);
	
	CombatTarget = nullptr;
}

void AStaticEnemy::SetTarget()
{
	if (CombatTarget)
	{
		FVector StartLocation = GetActorLocation();
		FVector TargetLocation = CombatTarget->GetActorLocation();
		FVector ShootDirection = (TargetLocation - StartLocation).GetSafeNormal();
		LaserTargetPosition = StartLocation + ShootDirection * LaserMaxRange;
		GetWorldTimerManager().SetTimer(LaserChargeUpTimer, this, &AStaticEnemy::LaserChargeUpComplete, ChargupAfterTargetSetTimer);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Target Set"));
	}
	else
	{
		PlayVFXCoolDown(GetActorLocation());
		PlayAudioCoolDown(GetActorLocation());
		GunState = ESGunState::ESGS_Cooling;
		GetWorldTimerManager().SetTimer(LaserCoolDownTimer, this, &AStaticEnemy::LaserCoolDownComplete, CooldownTimer);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Cooling down"));
	}
}

void AStaticEnemy::LaserChargeUpComplete()
{
	if (GunState != ESGunState::ESGS_Chargeing) return;

	GunState = ESGunState::ESGS_Shooting;

	FHitResult OutHit;
	SphereTrace(OutHit, GetActorLocation(), LaserTargetPosition, LaserRadius);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Shot at player"));

	PlayVFXAttack(GetActorLocation());
	PlayAudioAttack(GetActorLocation());

	PlayVFXCoolDown(GetActorLocation());
	PlayAudioCoolDown(GetActorLocation());
	GunState = ESGunState::ESGS_Cooling;
	GetWorldTimerManager().SetTimer(LaserCoolDownTimer, this, &AStaticEnemy::LaserCoolDownComplete, CooldownTimer);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Cooling down"));
}

void AStaticEnemy::LaserCoolDownComplete()
{
	if (GunState != ESGunState::ESGS_Cooling) return;

	GunState = ESGunState::ESGS_Idle;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Cooldown complete"));
	if (EnemyState == EEnemyState::EES_Attacking && CombatTarget)
	{
		GunState = ESGunState::ESGS_Chargeing;
		PlayVFXChargeUp(GetActorLocation());
		PlayAudioChargeUp(GetActorLocation());
		GetWorldTimerManager().SetTimer(LaserSetTargetTimer, this, &AStaticEnemy::SetTarget, ChargupUntilSetTargetTimer);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Started new targeting sequence"));
	}
}

void AStaticEnemy::SphereTrace(FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation,
	const float& traceRadius)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	UKismetSystemLibrary::SphereTraceSingle(
		this,
		StartLocation,
		EndLocation,
		traceRadius,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true);

	if (OutHit.GetActor())
	{
		UGameplayStatics::ApplyDamage(OutHit.GetActor(), Damage, GetController(), this, UDamageType::StaticClass());
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Should have killed player"));
	}
}

void AStaticEnemy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player && Player->ActorHasTag(FName("Player")))
	{
		CombatTarget = Player;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Detected player"));
		SeenAnEnemy();
	}
}

void AStaticEnemy::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	EnemyLeft();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player left range"));
}
