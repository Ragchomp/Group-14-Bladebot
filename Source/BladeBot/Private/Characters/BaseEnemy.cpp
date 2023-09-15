
#include "Characters/BaseEnemy.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"


ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ------------- Health ------------

void ABaseEnemy::HandleDamage(const float& DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReciveDamage(DamageAmount);

		if(Attributes->IsNotAlive())
		{
			Die();
		}
	}
}

void ABaseEnemy::Die()
{
	Tags.Add(FName("Dead"));
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	this->Destroy();
}

// ------------- Sensing ------------

bool ABaseEnemy::InTargetRange(const AActor* Target, const float Radius)
{
	if (Target == nullptr) return false;
	const float DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}

bool ABaseEnemy::InTargetRange(const FVector& Location, const float Radius)
{
	float DistanceToTarget = GetDistanceBetweenTwoPoints(Location, GetActorLocation());

	return DistanceToTarget <= Radius;
}

// ------------- VFX ------------

void ABaseEnemy::PlayVFXChargeUp(const FVector& PlayLocation)
{
	if (VFXChargeUp) {
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			VFXChargeUp,
			PlayLocation,
			GetActorRotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

void ABaseEnemy::PlayVFXCoolDown(const FVector& PlayLocation)
{
	if (VFXCoolDown) {
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			VFXCoolDown,
			PlayLocation,
			GetActorRotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

void ABaseEnemy::PlayVFXAttack(const FVector& PlayLocation)
{
	if (VFXAttack) {
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			VFXAttack,
			PlayLocation,
			GetActorRotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

// ------------- Audio ------------

void ABaseEnemy::PlayAudioGetHit(const FVector& Location)
{
	if(GetHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			GetHitSound,
			Location
		);
	}
}

void ABaseEnemy::PlayAudioAttack(const FVector& Location)
{
	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			AttackSound,
			Location
		);
	}
}

void ABaseEnemy::PlayAudioChargeUp(const FVector& Location)
{
	if (ChargeUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ChargeUpSound,
			Location
		);
	}
}

void ABaseEnemy::PlayAudioCoolDown(const FVector& Location)
{
	if (CoolDownSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			CoolDownSound,
			Location
		);
	}
}

// ------------- Other ------------

void ABaseEnemy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABaseEnemy::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ABaseEnemy::SphereTrace(FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation, const float& traceRadius)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	UKismetSystemLibrary::SphereTraceSingle(
		this,
		StartLocation,
		EndLocation,
		traceRadius,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true);
}

// ------------- Getters and Setters ------------

FVector ABaseEnemy::GetPointWithRotator(const FVector& Start, const FRotator& Rotation, float Distance)
{
	FQuat Quaternion = Rotation.Quaternion();
	FVector Direction = Quaternion.GetForwardVector();
	FVector Point = Start + (Direction * Distance);

	return Point;
}

FVector ABaseEnemy::GetVectorOfRotation(const FRotator& Rotation)
{
	FQuat Quaternion = Rotation.Quaternion();
	FVector Direction = Quaternion.GetForwardVector();

	return Direction;
}

FVector ABaseEnemy::GetVectorBetweenTwoPoints(const FVector& Point1, const FVector& Point2)
{
	FVector VectorBetweenLocations = Point1 - Point2;
	VectorBetweenLocations.Normalize();

	return -VectorBetweenLocations;
}

float ABaseEnemy::GetDistanceBetweenTwoPoints(const FVector& Point1, const FVector& Point2)
{
	float DistanceToTarget = FVector::Dist(Point1, Point2);

	return DistanceToTarget;
}


