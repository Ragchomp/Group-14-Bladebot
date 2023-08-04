
//Main
#include "GrapplingHook/GrapplingHookHead.h"

//Components
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AGrapplingHookHead::AGrapplingHookHead()
{
	PrimaryActorTick.bCanEverTick = true;

	// Mesh init
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Collision init
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	
}

void AGrapplingHookHead::BeginPlay()
{
	Super::BeginPlay();
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AGrapplingHookHead::OnOverlap);

	GrappleState = EGrappleState::EGS_InAir;

}

void AGrapplingHookHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move(DeltaTime);
}

void AGrapplingHookHead::Move(float DeltaTime)
{
	if (CanMove == false) return;
	if(GrappleState == EGrappleState::EGS_InAir)
	{
		FVector NewLocation = GetActorLocation();
		NewLocation += GetActorUpVector() * ProjectileSpeed * DeltaTime;
		SetActorLocation(NewLocation);
	}
}

void AGrapplingHookHead::Despawn()
{
	GrappleState = EGrappleState::EGS_Retracted;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	this->Destroy();
}

void AGrapplingHookHead::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FString OtherActorName = OtherActor->GetName();
	//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, OtherActorName);

	GrappleState = EGrappleState::EGS_Attached;

}
