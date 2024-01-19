// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPad.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

class APlayerCharacter;
// Sets default values
AJumpPad::AJumpPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionMesh = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionMesh"));
	SetRootComponent(CollisionMesh);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectiveMesh"));
	Mesh->SetupAttachment(CollisionMesh);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComp->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AJumpPad::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AJumpPad::OnOverlap);
	PlayVFXPassive(GetActorLocation());
	PlayAudioPassive(GetActorLocation());
}

// Called every frame
void AJumpPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateVFXLocationRotation();
}

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

void AJumpPad::throwPlayer(APlayerCharacter* Player, FVector ThrowAngle, float _throwPower)
{
	Player->PlayerMovementComponent->AddImpulse(ThrowAngle*_throwPower);
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
	if(Jumped == false)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("ShouldJumpPlayer"));

		if (Player && Player->ActorHasTag(FName("Player")))
		{
			throwPlayer(Player, throwAngle, throwPower);
		}
		Jumped = true;
		GetWorldTimerManager().SetTimer(JumpResetTimerHandeler, this, &AJumpPad::canJumpAgain, 1);
	}
}
