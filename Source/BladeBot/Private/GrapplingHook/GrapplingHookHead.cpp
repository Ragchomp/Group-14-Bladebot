
//Main
#include "GrapplingHook/GrapplingHookHead.h"

//Components
#include "Components/PlayerMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GrapplingHook/GrapplingRopeActor.h"

AGrapplingHookHead::AGrapplingHookHead()
{
	//set this actor to tick
	PrimaryActorTick.bCanEverTick = true;

	//initialize root component and projectile movement component
	WallHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("WallHitbox"));
	PlayerHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerHitbox"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("CustomProjectileMovementComponent"));

	//initialize the mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	//Setup Attachments
	SetRootComponent(WallHitbox);
	Mesh->SetupAttachment(GetRootComponent());
	PlayerHitbox->SetupAttachment(GetRootComponent());

	//set wall hitbox collision settings and size
	WallHitbox->SetNotifyRigidBodyCollision(true);
	WallHitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WallHitbox->SetCollisionObjectType(ECC_PhysicsBody);
	WallHitbox->SetSphereRadius(32);

	//set player hitbox collision settings and size
	PlayerHitbox->SetNotifyRigidBodyCollision(true);
	PlayerHitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlayerHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
	PlayerHitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PlayerHitbox->SetSphereRadius(50);

	//set mesh collision settings
	Mesh->SetCollisionProfileName("NoCollision");

	//set the projectile movement component settings
	ProjectileMovementComponent->ProjectileGravityScale = 0;
	ProjectileMovementComponent->InitialSpeed = 2000;
	ProjectileMovementComponent->MaxSpeed = 2000;
}

void AGrapplingHookHead::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//bind the onhit function to the WallHitbox's hit event
	WallHitbox->OnComponentHit.AddDynamic(this, &AGrapplingHookHead::OnHit);

	//bind the onoverlap function to the PlayerHitbox's overlap event
	PlayerHitbox->OnComponentBeginOverlap.AddDynamic(this, &AGrapplingHookHead::OnOverlapBegin);
	PlayerHitbox->OnComponentEndOverlap.AddDynamic(this, &AGrapplingHookHead::OnOverlapEnd);

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

	//set the projectile movement component's velocity to the forward vector of the actor
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;

	//array for player movement components
	TArray<UPlayerMovementComponent*> PlayerMovementComponents;

	//get all player movement components
	GetInstigator()->GetComponents(PlayerMovementComponents);

	//check if we have a player movement component
	if (PlayerMovementComponents.Num() > 0)
	{
		//set the player movement component
		PlayerMovementComponent = PlayerMovementComponents[0];

		//get the max distance from the player movement component
		MaxDistance = PlayerMovementComponent->MaxGrappleDistance;
	}

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

	//check if we should destroy on impact and if we've hit a wall
	if (bDestroyOnImpact && bHasHitWall)
	{
		//destroy ourselves
		DoDestroy();
	}
}

void AGrapplingHookHead::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if otheractor is not the instigator of this actor return
	if (OtherActor != GetInstigator())
	{
		//print debug message and return
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "HookHead's PlayerHitbox overlapped by something other than player");
		return;
	}

	//don't do anything if we can't despawn by overlap
	if (!bCanDespawnbyOverlap)
	{
		return;
	}

	//check if we've been alive for the minimum time
	if (GetWorld()->GetTimeSeconds() - CreationTime < MinTimeAlive)
	{
		//prevent overlap events from destroying us
		bCanDespawnbyOverlap = false;

		//don't do anything else
		return;
	}

	//check if we have a player movement component
	if (PlayerMovementComponent)
	{
		//start player grapple
		PlayerMovementComponent->StopGrapple();
	}

	//destroy ourselves
	Destroy();
}

void AGrapplingHookHead::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//check if otheractor is the instigator of this actor
	if (OtherActor == GetInstigator())
	{
		//set bCanDespawnbyOverlap to true
		bCanDespawnbyOverlap = true;
	}
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

	//check if we should destroy on impact
	if (bDestroyOnImpact)
	{
		//check if we shouldn't destroy ourselves immediately
		if (DestroyDelay != 0)
		{
			//set the timer to destroy ourselves
			GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &AGrapplingHookHead::DoDestroy, DestroyDelay);
		}
		else
		{
			//destroy ourselves immediately
			Destroy();	
		}
	}

	//set bHasHitWall to true
	bHasHitWall = true;
}

void AGrapplingHookHead::DoDestroy()
{
	//destroy ourselves
	Destroy();

	//check if we have a player movement component
	if (PlayerMovementComponent)
	{
		//stop the player grapple
		PlayerMovementComponent->StopGrapple();
	}
}
