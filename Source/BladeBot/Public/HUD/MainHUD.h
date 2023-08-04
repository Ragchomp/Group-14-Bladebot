#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

UCLASS()
class BLADEBOT_API AMainHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void PreInitializeComponents() override;


public:
	UPROPERTY(EditDefaultsOnly,Category = "Player")
	TSubclassOf<class UPlayerOverlay> PlayerOverlayClass;

	UPROPERTY(VisibleAnywhere)
	UPlayerOverlay* PlayerOverlay;

	FORCEINLINE UPlayerOverlay* GetMainOverlay() {  return PlayerOverlay; }

};
