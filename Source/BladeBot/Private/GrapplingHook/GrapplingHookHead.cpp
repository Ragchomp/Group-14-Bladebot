
//Main
#include "GrapplingHook/GrapplingHookHead.h"

//Components
#include "Components/CustomProjectileMovementComponent.h"
#include "Components/PlayerMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"

AGrapplingHookHead::AGrapplingHookHead()
{
	PrimaryActorTick.bCanEverTick = true;

	//initialize root component and projectile movement component
	Hitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Hitbox"));
	ProjectileMovementComponent = CreateDefaultSubobject<UCustomProjectileMovementComponent>(TEXT("CustomProjectileMovementComponent"));

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

	//set default max distance
	ProjectileMovementComponent->MaxDistance = 3000.f;

	//enable checking for max distance
	ProjectileMovementComponent->bUseMaxDistance = true;

	//set use spawn as distance check location to true
	ProjectileMovementComponent->bSetDistanceCheckLocationOnSpawn = true;

	//default to fixed speed
	ProjectileMovementComponent->bFixedSpeed = true;
}

void AGrapplingHookHead::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//bind the onhit function to the Hitbox's hit event
	Hitbox->OnComponentHit.AddDynamic(this, &AGrapplingHookHead::OnHit);

	//bind the onoverlap function to the Hitbox's overlap event
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AGrapplingHookHead::OnOverlap);

	//check if we should use max distance
	if (ProjectileMovementComponent->bUseMaxDistance)
	{
		//bind the OnMaxDistReached delegate to the Despawn function
		ProjectileMovementComponent->OnMaxDistReached.BindUObject(this, &AGrapplingHookHead::Despawn);
	}

	//set the projectile movement component's properties
	ProjectileMovementComponent->MaxSpeed = InitialVelocity.Size();
	ProjectileMovementComponent->Velocity = InitialVelocity;

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
}

void AGrapplingHookHead::Despawn()
{
	//attach the grappling hook head to the player
	this->AttachToActor(GetInstigator(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	//set the grapple state to retracted
	GrappleState = EGrappleState::EGS_Retracted;

	//hide this actor
	SetActorHiddenInGame(true);

	//disable collision
	SetActorEnableCollision(false);

	//disable tick
	ProjectileMovementComponent->bDoTick = false;
	RopeActor->bDoTick = false;

	//set can move to false
	ProjectileMovementComponent->bCanMove = false;
}

void AGrapplingHookHead::Reactivate(const FVector NewVelocity)
{
	//detach the grappling hook head from the player
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//set the grapple state to in air
	GrappleState = EGrappleState::EGS_InAir;

	//show this actor
	SetActorHiddenInGame(false);

	//enable collision
	SetActorEnableCollision(true);

	//reenable tick for the projectile movement component and the rope actor
	ProjectileMovementComponent->bDoTick = true;
	RopeActor->bDoTick = true;

	//set the projectile movement component's properties
	ProjectileMovementComponent->SetDistanceCheckLocation(GetInstigator()->GetActorLocation());
	ProjectileMovementComponent->Velocity = NewVelocity;
	ProjectileMovementComponent->InitialSpeed = NewVelocity.Size();
	ProjectileMovementComponent->MaxSpeed = NewVelocity.Size();
	ProjectileMovementComponent->bCanMove = true;

	//get the instigator's location
	FVector InstigatorLocation = GetInstigator()->GetActorLocation();

	//set the location of the grappling hook head to the instigator's location
	this->SetActorLocation(InstigatorLocation + NewVelocity.GetSafeNormal() * SpawnDistance);
}

void AGrapplingHookHead::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//de-spawn the grappling hook head
	Despawn();
}

void AGrapplingHookHead::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//set the grapple state to attached
	GrappleState = EGrappleState::EGS_Attached;

	//disable movement
	ProjectileMovementComponent->bCanMove = false;

	//check if we have a player movement component
	if (PlayerMovementComponent)
	{
		//start player grapple
		PlayerMovementComponent->StartGrapple(RopeActor);
	}
}
