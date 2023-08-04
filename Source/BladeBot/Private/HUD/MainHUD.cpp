#include "HUD/MainHUD.h"
#include "HUD/PlayerOverlay.h"

void AMainHUD::PreInitializeComponents()
{
	// Adds the Hud overlay to the screen through the Gamemode(World), then through the player controller
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && PlayerOverlayClass)
		{
			PlayerOverlay = CreateWidget<UPlayerOverlay>(Controller, PlayerOverlayClass);
			PlayerOverlay->AddToViewport();
		}
	}

}
