
// Classes
#include "Characters/Enemies/MissleDestructable_Boss.h"
#include "Characters/Enemies/BossEnemy.h"
#include "Components/AttributeComponent.h"

// Components
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"


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

void AMissleDestructable_Boss::SetCombatTarget(AActor* CombatTargetInn)
{
	CombatTarget = CombatTargetInn;
}


void AMissleDestructable_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if( /* line of sight */)
		// rotate to player

	//else
		// branch line of sight, find path to player  
		// rotate toward toward pathes
		// re run check

		// move
	if (CombatTarget)
	{
		CheckSight();
	}

	Rotate(CombatTarget->GetActorLocation(), DeltaTime);
	Move(DeltaTime);
}

float AMissleDestructable_Boss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	//GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Take Damage"));
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


void AMissleDestructable_Boss::Die()
{
	Tags.Add(FName("Dead"));
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	this->Destroy();
}

void AMissleDestructable_Boss::Rotate(FVector Target, float DeltaTime)
{
	if (CombatTarget)
	{
		FVector Point1 = GetActorLocation();
		FVector Point2 = Target;
		FVector MovementVector = GetVectorBetweenTwoPoints(Point1, Point2);

		FRotator CurrentRotation = GetActorRotation();
		FRotator TargetRotation = MovementVector.Rotation();

		FRotator NewRotation = FMath::Lerp(CurrentRotation, TargetRotation, RotationSpeed);
		SetActorRotation(NewRotation);
	}
}

void AMissleDestructable_Boss::Move(float DeltaTime)
{
	FVector NewLocation = GetActorLocation();
	NewLocation += GetActorForwardVector() * MovementSpeed * DeltaTime;
	SetActorLocation(NewLocation);
}

bool AMissleDestructable_Boss::CheckSight()
{

	if(CombatTarget)
	{
		FHitResult OutHit;
		LineTrace(CombatTarget->GetActorLocation(), OutHit);
	}

	return false;
}

void AMissleDestructable_Boss::LineTrace(FVector Target, FHitResult& OutHit)
{
	// Trace Information
	const FVector Start = GetActorLocation();
	const FVector End = Target;
	bool bTraceComplex = false;
	FHitResult LineHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// The Trace
	UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		OutHit,
		true
	);
}

FVector AMissleDestructable_Boss::GetVectorBetweenTwoPoints(const FVector& Point1, const FVector& Point2)
{
	FVector VectorBetweenLocations = Point1 - Point2;
	VectorBetweenLocations.Normalize();

	return -VectorBetweenLocations;
}

