// Fill out your copyright notice in the Description page of Project Settings.



// Classes
#include "Characters/Enemies/BossEnemy.h"
#include "Characters/PlayerCharacter.h"

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
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	EnemyState = EEnemyState::EES_Patroling;

	Tags.Add(FName("Enemy"));

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::OnOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ABossEnemy::EndOverlap);
}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossEnemy::SeenAnEnemy()
{

}

void ABossEnemy::EnemyLeft()
{

	CombatTarget = nullptr;
	EnemyState = EEnemyState::EES_Idle;
}

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
