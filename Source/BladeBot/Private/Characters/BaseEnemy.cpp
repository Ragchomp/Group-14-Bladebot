
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

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComp->SetupAttachment(GetRootComponent());
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateVFXLocationRotation();

}

float ABaseEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);

		if (Attributes->IsNotAlive())
		{
			Die();
		}
	}
	return 0.0f;
}

// ------------- Health ------------

void ABaseEnemy::Die()
{
	if (CanDie == false) return;

	PlayAudioDie(GetActorLocation());
	PlayVFXDie(GetActorLocation());

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

void ABaseEnemy::UpdateVFXLocationRotation()
{
	if (NiagaraComp)
	{
		NiagaraComp->SetWorldLocation(GetActorLocation());
		NiagaraComp->SetWorldRotation(GetActorRotation());
	}
}

void ABaseEnemy::PlayVFXChargeUp(const FVector& PlayLocation)
{
	if (VFXChargeUp && NiagaraComp) {
		NiagaraComp->SetAsset(VFXChargeUp);

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
	if (VFXCoolDown && NiagaraComp) {
		NiagaraComp->SetAsset(VFXCoolDown);

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
	if (VFXAttack && NiagaraComp) {
		NiagaraComp->SetAsset(VFXAttack);

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

void ABaseEnemy::PlayVFXDie(const FVector& PlayLocation)
{
	if (VFXDie && NiagaraComp) {
		NiagaraComp->SetAsset(VFXDie);

		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			VFXDie,
			PlayLocation,
			GetActorRotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

void ABaseEnemy::PlayVFXSpawn(const FVector& PlayLocation)
{
	if (VFXSpawn && NiagaraComp) {
		NiagaraComp->SetAsset(VFXSpawn);

		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			VFXSpawn,
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

void ABaseEnemy::PlayAudioDie(const FVector& Location)
{
	if (DieSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DieSound,
			Location
		);
	}
}

void ABaseEnemy::PlayAudioSpawn(const FVector& Location)
{
	if (SpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			SpawnSound,
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
