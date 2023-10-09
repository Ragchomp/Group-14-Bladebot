// Fill out your copyright notice in the Description page of Project Settings.



// Classes
#include "Characters/Enemies/BossEnemy.h"
#include "Characters/PlayerCharacter.h"
#include "Characters/Enemies/MissleDestructable_Boss.h"

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

}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ------------ Health ----------------


// handle damage calls jump
// disable handle damage until landed at new location

// ------------ Movement ----------------

// jumping logic to move to next zone

// ------------ Combat ----------------

void ABossEnemy::SeenAnEnemy()
{
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("enemy Seen"));
	GunState = ESGunState::ESGS_Chargeing;
	PlayVFXChargeUp(GetActorLocation());
	PlayAudioChargeUp(GetActorLocation());
	GetWorldTimerManager().SetTimer(RocketBarrageStartupTimer, this, &ABossEnemy::ShootRocketBarrage, RocketBarrageStartupTime);
}

void ABossEnemy::ShootRocketBarrage()
{
	if (RocketsShoot < 1)
	{
		GunState = ESGunState::ESGS_Shooting;
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green, TEXT("Started barrage"));

		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Spanwned rocket start timer"));
		GetWorldTimerManager().SetTimer(RocketBarrageNextRocketTimer, this, &ABossEnemy::SpawnRocket, NextRocketIntervalTime);
		RocketsShoot++;
	}
	else
	{
		RocketsShoot = 0;
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, TEXT("Started cooldown"));
		GunState = ESGunState::ESGS_Cooling;
		GetWorldTimerManager().SetTimer(RocketBarrageCooldownTimer, this, &ABossEnemy::RocketBarrageCooldown, RocketBarrageCooldownTime);
	}

}

void ABossEnemy::SpawnRocket()
{
	//VFX and Audio
	PlayVFXAttack(GetActorLocation());
	PlayAudioAttack(GetActorLocation());
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Shot a rocket"));

	// Spawn rocket
	if(MissleDestuctable_BP)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(-100.f,0.f,1000.f);
		//SpawnLocation + FVector(500.f, 500.f, 500.f);
		//SpawnLocation + FVector(FMath::RandRange(10, 100), FMath::RandRange(10, 100), FMath::RandRange(10, 100));

		FRotator SpawnRotation = GetActorRotation();

		GetWorld()->SpawnActor<AMissleDestructable_Boss>(MissleDestuctable_BP, SpawnLocation, GetActorRotation());
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

// shooting

void ABossEnemy::EnemyLeft()
{
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Enemy left"));
	CombatTarget = nullptr;
	EnemyState = EEnemyState::EES_Idle;

	// Shooting timer

}

// ------------ Misc ----------------

void ABossEnemy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player && Player->ActorHasTag(FName("Player")))
	{
		CombatTarget = Player;
		SeenAnEnemy();
	}
}

void ABossEnemy::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EnemyLeft();
}
