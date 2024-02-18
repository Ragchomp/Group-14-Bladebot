
// Classes
#include "Objectives/ObjectivePoint.h"

// Components
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AObjectivePoint::AObjectivePoint()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionMesh = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionMesh"));
	SetRootComponent(CollisionMesh);

	ObjectveMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ObjectiveMesh"));
	ObjectveMesh->SetupAttachment(CollisionMesh);

	AfterActivationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AfterActivationMesh"));
	AfterActivationMesh->SetupAttachment(CollisionMesh);
	AfterActivationMesh->SetVisibility(false);
	AfterActivationMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DisabledMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisabledMesh"));
	DisabledMesh->SetupAttachment(CollisionMesh);
	DisabledMesh->SetVisibility(false);
	DisabledMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComp->SetupAttachment(GetRootComponent());
}

void AObjectivePoint::BeginPlay()
{
	Super::BeginPlay();


	Tags.Add(FName("Object"));
	PlayAudioPassive(GetActorLocation());
	PlayVFXPassive(GetActorLocation());

	if(isDisabled == true)
		SetInactive();

	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AObjectivePoint::OnOverlap);
}

void AObjectivePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// updates live location/rotation for VFX
	UpdateVFXLocationRotation();
}

void AObjectivePoint::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!DamageToComplete)
	{

		// if player touches the overlap mesh it becomes active and changes to the active mesh if enabled through player character.
		if (AlreadyHit == false)
		{
			APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

			if (Player && Player->ActorHasTag(FName("Player")) && isDisabled == false)
			{
				Tags.Add(FName("ObjectiveComplete"));

				PlayAudioChange(GetActorLocation());
				PlayVFXChange(GetActorLocation());
				AlreadyHit = true;
				ObjectiveComplete = true;

				if (shouldBeDestroyed)
				{
					ObjectveMesh->SetVisibility(false);
					ObjectveMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					AfterActivationMesh->SetVisibility(true);
					AfterActivationMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					this->Destroy();
				}

				Player->CheckIfObjectivesComplete(this);
			}
		}
	}

}

float AObjectivePoint::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(DamageToComplete)
	{
		// if player touches the overlap mesh it becomes active and changes to the active mesh if enabled through player character.
		if (AlreadyHit == false)
		{
			APlayerCharacter* Player = Cast<APlayerCharacter>(DamageCauser);

			if (Player && Player->ActorHasTag(FName("Player")) && isDisabled == false)
			{
				Tags.Add(FName("ObjectiveComplete"));

				PlayAudioChange(GetActorLocation());
				PlayVFXChange(GetActorLocation());
				AlreadyHit = true;
				ObjectiveComplete = true;

				if (shouldBeDestroyed)
				{
					ObjectveMesh->SetVisibility(false);
					ObjectveMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					AfterActivationMesh->SetVisibility(true);
					AfterActivationMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					this->Destroy();
				}

				Player->CheckIfObjectivesComplete(this);
			}
		}
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Took damage"));
	return 0.0f;
}

void AObjectivePoint::SetInactive()
{
	if (ActorHasTag(FName("ObjectiveComplete"))) return;
	
	ObjectveMesh->SetVisibility(false);
	ObjectveMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AfterActivationMesh->SetVisibility(false);
	AfterActivationMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisabledMesh->SetVisibility(true);
	DisabledMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Tags.Add(FName("ObjectiveDisabled"));
	isDisabled = true;
}

void AObjectivePoint::SetActive()
{
	ObjectveMesh->SetVisibility(true);
	ObjectveMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AfterActivationMesh->SetVisibility(false);
	AfterActivationMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisabledMesh->SetVisibility(false);
	DisabledMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if(this->ActorHasTag("ObjectiveDisabled"))
		Tags.Remove("ObjectiveDisabled");
	isDisabled = false;
}

// VFX ------------------------------------------

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

