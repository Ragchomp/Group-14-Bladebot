// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainHUD.h"
#include "HUD/PlayerOverlay.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("MainHUD INIT"));

	UWorld* World = GetWorld();
	if(World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("MainHud Got Wordld"));

		APlayerController* Controller = World->GetFirstPlayerController();
		if(Controller && PlayerOverlayClass)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("MainHud got Controller"));

			PlayerOverlay = CreateWidget<UPlayerOverlay>(Controller, PlayerOverlayClass);
			PlayerOverlay->AddToViewport();
		}
	}
}
