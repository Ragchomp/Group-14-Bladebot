// Fill out your copyright notice in the Description page of Project Settings.

#include "BladebotVictoryBox.h"

#include "BladebotGameMode.h"
#include "BladebotKillBox.h"
#include "Characters/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABladebotVictoryBox::ABladebotVictoryBox()
{
	PrimaryActorTick.bCanEverTick = true;

	VictoryBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KillBoxMesh"));
	VictoryBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("KillBoxCollision"));
	SetRootComponent(VictoryBoxCollision);
	VictoryBoxMesh->AttachToComponent(VictoryBoxCollision, FAttachmentTransformRules::KeepRelativeTransform);

	VictoryBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABladebotVictoryBox::OnOverlapBegin);
	VictoryBoxCollision->SetCollisionProfileName("OverlapAll");
	VictoryBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	VictoryBoxCollision->SetGenerateOverlapEvents(true);
}

void ABladebotVictoryBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndexType, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}