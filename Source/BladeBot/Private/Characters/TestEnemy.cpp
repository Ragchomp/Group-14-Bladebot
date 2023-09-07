
// Classes
#include "Characters/TestEnemy.h"
#include "AIController.h"

// Components
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

ATestEnemy::ATestEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh Init
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -310.f));
	GetMesh()->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));

	// Capsule Init
	GetCapsuleComponent()->SetCapsuleHalfHeight(330);
	GetCapsuleComponent()->SetCapsuleRadius(160);

}

void ATestEnemy::BeginPlay()
{
	Super::BeginPlay();
	// Gets the AI Controller
	EnemyController = Cast<AAIController>(GetController());


}

void ATestEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}