
//Main
#include "GrapplingHook/GrapplingHookHead.h"

//Components
#include "Components/PlayerMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"

AGrapplingHookHead::AGrapplingHookHead()
{
	PrimaryActorTick.bCanEverTick = true;

	//initialize root component and projectile movement component
	Hitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Hitbox"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("CustomProjectileMovementComponent"));

	//set the root component
	SetRootComponent(Hitbox);

	//initialize the mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	//attach the mesh to the root component
	Mesh->SetupAttachment(Hitbox);

	//set hitbox simulation generates hit events to true
	Hitbox->SetNotifyRigidBodyCollision(true);

	//set hitbox collision settings
	Hitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Hitbox->SetCollisionProfileName("PhysicsActor");

	//set the projectile movement component settings
	ProjectileMovementComponent->ProjectileGravityScale = 0;
	ProjectileMovementComponent->InitialSpeed = 2000;
	ProjectileMovementComponent->MaxSpeed = 2000;
}

void AGrapplingHookHead::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//bind the onhit function to the Hitbox's hit event
	Hitbox->OnComponentHit.AddDynamic(this, &AGrapplingHookHead::OnHit);

	//bind the onoverlap function to the Hitbox's overlap event
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AGrapplingHookHead::OnOverlap);

	//set the grapple state to in air
	GrappleState = EGrappleState::EGS_InAir;

	//spawn parameters for the rope actor
	FActorSpawnParameters SpawnParameters;

	//set the owner of the rope actor to this actor
	SpawnParameters.Owner = this;

	//set the instigator of the rope actor to the instigator of this actor
	SpawnParameters.Instigator = GetInstigator();

	//spawn the rope actor
	RopeActor = GetWorld()->SpawnActor<AGrapplingRopeActor>(RopeActorClass, GetActorLocation(), GetActorRotation(), SpawnParameters);

	//check if we should use the distance check function and whether we should use the spawn locaion for the distance checks
	if (bUseMaxDistance && bUseSpawnForDistanceCheck)
	{
		//set the distance check location to the spawn location
		DistanceCheckLocation = GetInstigator()->GetActorLocation();
	}

	ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
}

void AGrapplingHookHead::Tick(float DeltaTime)
{
	//call the parent implementation
	Super::Tick(DeltaTime);

	//check if we should check max distance
	if (bUseMaxDistance)
	{
		//check if we've reached the max distance
		if (FVector::Dist(DistanceCheckLocation, GetActorLocation()) >= MaxDistance)
		{
			//destroy ourselves
			Destroy();
		}
	}
}

void AGrapplingHookHead::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//check if we have a player movement component
	if (PlayerMovementComponent)
	{
		//stop the player grapple
		PlayerMovementComponent->StopGrapple();

		//set the grapple state to retracted
		GrappleState = EGrappleState::EGS_Retracted;
	}

	//destroy ourselves
	Destroy();
}

void AGrapplingHookHead::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//set the grapple state to attached
	GrappleState = EGrappleState::EGS_Attached;

	//check if we have a player movement component
	if (PlayerMovementComponent)
	{
		//start player grapple
		PlayerMovementComponent->StartGrapple(RopeActor);
	}
}
