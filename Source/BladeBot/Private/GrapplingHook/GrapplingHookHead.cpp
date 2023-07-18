
#include "GrapplingHook/GrapplingHookHead.h"
#include "Components/StaticMeshComponent.h"

#include "StaticMeshDescription.h"


AGrapplingHookHead::AGrapplingHookHead()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh init
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Collision Init
	//Mesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//Mesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//Mesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//Mesh()->SetGenerateOverlapEvents(true);

}

void AGrapplingHookHead::BeginPlay()
{
	Super::BeginPlay();
	
}


void AGrapplingHookHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move(DeltaTime);

	//Despawn Timer
	ProjectileLife += DeltaTime;
	if (ProjectileLife > ProjectileMaxLife)
	{
		Despawn();
	}

}

void AGrapplingHookHead::Move(float DeltaTime)
{
	FVector NewLocation = GetActorLocation();
	NewLocation += GetActorUpVector() * ProjectileSpeed * DeltaTime;



	SetActorLocation(NewLocation);
}

void AGrapplingHookHead::Despawn()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	this->Destroy();
}