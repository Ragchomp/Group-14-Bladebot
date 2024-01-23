// classes
#include "DestroyableObject.h"

// Components 
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ADestroyableObject::ADestroyableObject()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionMesh = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionMesh"));
	SetRootComponent(CollisionMesh);

	ActiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectiveMesh"));
	ActiveMesh->SetupAttachment(CollisionMesh);

	ChangedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AfterActivationMesh"));
	ChangedMesh->SetupAttachment(CollisionMesh);
	ChangedMesh->SetVisibility(false);
	ChangedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComp->SetupAttachment(GetRootComponent());

}

void ADestroyableObject::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Object"));
	PlayAudioPassive(GetActorLocation());
	PlayVFXPassive(GetActorLocation());
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ADestroyableObject::OnOverlap);
}

void ADestroyableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Updates VFX location/rotation live
	UpdateVFXLocationRotation();
}

void ADestroyableObject::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// changes mesh if player has hit it with a spin attack.
	if (alreadyHit == false)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player && Player->ActorHasTag(FName("Player")) && Player->bIsSpinAttacking)
		{
			ChangeMesh();
		}
	}
}

void ADestroyableObject::ChangeMesh()
{
	// changes mesh from active to other mesh
	Tags.Add(FName("UpdatedMesh"));
	ActiveMesh->SetVisibility(false);
	ActiveMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChangedMesh->SetVisibility(true);
	ChangedMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	PlayAudioChange(GetActorLocation());
	PlayVFXChange(GetActorLocation());
	alreadyHit = true;
}

// VFX -------------------------------------------

void ADestroyableObject::UpdateVFXLocationRotation()
{
	if (NiagaraComp)
	{
		NiagaraComp->SetWorldLocation(GetActorLocation());
		NiagaraComp->SetWorldRotation(GetActorRotation());
	}
}

void ADestroyableObject::PlayVFXPassive(const FVector& PlayLocation)
{
	if (VFXPassive && NiagaraComp) {
		NiagaraComp->SetAsset(VFXPassive);

		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			VFXPassive,
			PlayLocation,
			GetActorRotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

void ADestroyableObject::PlayVFXChange(const FVector& PlayLocation)
{
	if (VFXChange && NiagaraComp) {
		NiagaraComp->SetAsset(VFXChange);

		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			VFXChange,
			PlayLocation,
			GetActorRotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

void ADestroyableObject::PlayVFXChangedPassive(const FVector& PlayLocation)
{
	if (VFXChangedPassive && NiagaraComp) {
		NiagaraComp->SetAsset(VFXChangedPassive);

		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			VFXChangedPassive,
			PlayLocation,
			GetActorRotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

void ADestroyableObject::PlayAudioPassive(const FVector& Location)
{
	if (PassiveSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PassiveSound,
			Location
		);
	}
}

void ADestroyableObject::PlayAudioChange(const FVector& Location)
{
	if (ChangedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ChangedSound,
			Location
		);
	}
}

void ADestroyableObject::PlayAudioChangedPassive(const FVector& Location)
{
	if (ChangedPassiveSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ChangedPassiveSound,
			Location
		);
	}
}

