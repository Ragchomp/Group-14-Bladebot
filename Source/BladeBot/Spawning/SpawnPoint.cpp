

#include "SpawnPoint.h"

#include "EngineUtils.h"

ASpawnPoint::ASpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualComponent"));
	SetRootComponent(SpawnBoxMesh);
}

void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();

}

void ASpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

