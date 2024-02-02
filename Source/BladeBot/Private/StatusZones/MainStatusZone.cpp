

#include "StatusZones/MainStatusZone.h"
#include "Characters/PlayerCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AMainStatusZone::AMainStatusZone()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Mesh"));
	CollisionBox->SetupAttachment(GetRootComponent());

}

void AMainStatusZone::BeginPlay()
{
	Super::BeginPlay();
	PlayVFXPassive(GetActorLocation());
	PlayAudioPassive(GetActorLocation());
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMainStatusZone::OnOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AMainStatusZone::EndOverlap);
}

void AMainStatusZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateVFXLocationRotation();

}

void AMainStatusZone::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (alreadyHit == false)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player && Player->ActorHasTag(FName("Player")))
		{
			GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("HitPlayer Starts")));
			alreadyHit = true;
		}
	}
}

void AMainStatusZone::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (alreadyHit == true)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player && Player->ActorHasTag(FName("Player")))
		{
			GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("Hitplayer ends")));
			alreadyHit = false;
		}
	}
}



void AMainStatusZone::UpdateVFXLocationRotation()
{
	if (NiagaraComp)
	{
		NiagaraComp->SetWorldLocation(GetActorLocation());
		NiagaraComp->SetWorldRotation(GetActorRotation());
	}
}

void AMainStatusZone::PlayVFXPassive(const FVector& PlayLocation)
{
	if (PassiveVFX && NiagaraComp) {
		NiagaraComp->SetAsset(PassiveVFX);

		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PassiveVFX,
			PlayLocation,
			GetActorRotation(),
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}
}

void AMainStatusZone::PlayAudioPassive(const FVector& Location)
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


