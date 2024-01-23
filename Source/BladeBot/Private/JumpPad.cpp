
// Classes
#include "JumpPad.h"

// Components
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

AJumpPad::AJumpPad()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionMesh = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionMesh"));
	SetRootComponent(CollisionMesh);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectiveMesh"));
	Mesh->SetupAttachment(CollisionMesh);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComp->SetupAttachment(GetRootComponent());
}

void AJumpPad::BeginPlay()
{
	Super::BeginPlay();

	PlayVFXPassive(GetActorLocation());
	PlayAudioPassive(GetActorLocation());
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AJumpPad::OnOverlap);
}

void AJumpPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateVFXLocationRotation();
}


void AJumpPad::throwPlayer(APlayerCharacter* Player, FVector ThrowAngle, float _throwPower)
{
	Player->PlayerMovementComponent->AddImpulse(ThrowAngle * _throwPower);
	PlayVFXActivate(GetActorLocation());
	PlayAudioActivate(GetActorLocation());
}

void AJumpPad::canJumpAgain()
{
	Jumped = false;
}

void AJumpPad::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Jumped == false)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player && Player->ActorHasTag(FName("Player")))
		{
			// Throws player if it touches collision mesh. 
			throwPlayer(Player, throwAngle, throwPower);
		}
		// Sets jumped bool so that function does not repeat.
		Jumped = true;
		// Resets Jumped to false when x seconds has gone. 
		GetWorldTimerManager().SetTimer(JumpResetTimerHandeler, this, &AJumpPad::canJumpAgain, JumppadCooldown);
	}
}

// VFX -----------------------------

void AJumpPad::UpdateVFXLocationRotation()
{
	if (NiagaraComp)
	{
		NiagaraComp->SetWorldLocation(GetActorLocation());
		NiagaraComp->SetWorldRotation(GetActorRotation());
	}
}

void AJumpPad::PlayVFXPassive(const FVector& PlayLocation)
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

void AJumpPad::PlayVFXActivate(const FVector& PlayLocation)
{
	if (VFXActivate && NiagaraComp) {
		NiagaraComp->SetAsset(VFXActivate);

		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			VFXActivate,
			PlayLocation,
			GetActorRotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

void AJumpPad::PlayAudioPassive(const FVector& Location)
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

void AJumpPad::PlayAudioActivate(const FVector& Location)
{
	if (ActivateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ActivateSound,
			Location
		);
	}
}
