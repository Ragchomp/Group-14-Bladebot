
// Classes
#include "Characters/Enemies/MissleDestructable_Boss.h"
#include "Components/AttributeComponent.h"

// Components
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"


AMissleDestructable_Boss::AMissleDestructable_Boss()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	SetRootComponent(Collision);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	Mesh->SetupAttachment(Collision);

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
}

void AMissleDestructable_Boss::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Enemy"));

}

void AMissleDestructable_Boss::Die()
{
	Tags.Add(FName("Dead"));
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	this->Destroy();
}

void AMissleDestructable_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// movement
	FVector NewLocation = GetActorLocation();
	NewLocation += GetActorForwardVector() * 100.f * DeltaTime;
	SetActorLocation(NewLocation);
}

float AMissleDestructable_Boss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Take Damage"));
	if (AttributeComponent)
	{
		AttributeComponent->ReceiveDamage(DamageAmount);

		if (AttributeComponent->IsNotAlive())
		{
			Die();
		}
	}
	return 0.0f;
}

