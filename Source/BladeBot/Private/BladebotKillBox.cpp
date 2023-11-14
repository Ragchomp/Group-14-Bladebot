// Fill out your copyright notice in the Description page of Project Settings.

#include "BladebotKillBox.h"
#include "Components/BoxComponent.h"

#include "BladebotGameMode.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABladebotKillBox::ABladebotKillBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KillBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KillBoxMesh"));
	KillBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("KillBoxCollision"));
	SetRootComponent(KillBoxCollision);
	KillBoxMesh->AttachToComponent(KillBoxCollision, FAttachmentTransformRules::KeepRelativeTransform);

	KillBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABladebotKillBox::OnOverlapBegin);
	KillBoxCollision->SetCollisionProfileName("OverlapAll");
	KillBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KillBoxCollision->SetGenerateOverlapEvents(true);
}

void ABladebotKillBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndexType, bool bFromSweep, const FHitResult& SweepResult)
{
	//Kill player
	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		if (const TObjectPtr<UWorld> World = GetWorld())
		{
			if (const TObjectPtr<ABladebotGameMode> GameMode = Cast<ABladebotGameMode>(World->GetAuthGameMode()))
			{
				UGameplayStatics::SetGamePaused(GetWorld(), true);
				//Cast<APlayerCharacter>(OtherActor)->CallRestartPlayer();
				//UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();

				//if (ViewportClient)
				//{
				//	ViewportClient->SetSuppressTransitionMessage(true);
				//}
			}
		}
	}
}