
#include "Objectives/ObjectivePoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AObjectivePoint::AObjectivePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionMesh = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionMesh"));
	SetRootComponent(CollisionMesh);

	ObjectveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectiveMesh"));
	ObjectveMesh->SetupAttachment(CollisionMesh);

	AfterActivationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AfterActivationMesh"));
	AfterActivationMesh->SetupAttachment(CollisionMesh);
	AfterActivationMesh->SetVisibility(false);
	AfterActivationMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComp->SetupAttachment(GetRootComponent());
}

void AObjectivePoint::BeginPlay()
{
	Super::BeginPlay();
	// only for now since this is what the player uses to detect. Will be changed once player is availible.
	Tags.Add(FName("Enemy"));
	PlayAudioPassive(GetActorLocation());
	PlayVFXPassive(GetActorLocation());

	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AObjectivePoint::OnOverlap);
}

void AObjectivePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateVFXLocationRotation();
}

void AObjectivePoint::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (AlreadyHit == false)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player && Player->ActorHasTag(FName("Player")))
		{
			Tags.Add(FName("ObjectiveComplete"));
			ObjectveMesh->SetVisibility(false);
			ObjectveMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			AfterActivationMesh->SetVisibility(true);
			AfterActivationMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			PlayAudioChange(GetActorLocation());
			PlayVFXChange(GetActorLocation());
			AlreadyHit = true;
			ObjectiveComplete = true;

			Player->CheckIfObjectivesComplete(this);
		}
		
	}
}

void AObjectivePoint::UpdateVFXLocationRotation()
{
	if (NiagaraComp)
	{
		NiagaraComp->SetWorldLocation(GetActorLocation());
		NiagaraComp->SetWorldRotation(GetActorRotation());
	}
}

void AObjectivePoint::PlayVFXPassive(const FVector& PlayLocation)
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

void AObjectivePoint::PlayVFXChange(const FVector& PlayLocation)
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

void AObjectivePoint::PlayVFXChangedPassive(const FVector& PlayLocation)
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

void AObjectivePoint::PlayAudioPassive(const FVector& Location)
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

void AObjectivePoint::PlayAudioChange(const FVector& Location)
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

void AObjectivePoint::PlayAudioChangedPassive(const FVector& Location)
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

