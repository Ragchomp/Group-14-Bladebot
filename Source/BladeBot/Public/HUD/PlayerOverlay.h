#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlay.generated.h"

UCLASS()
class BLADEBOT_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	/** Functions*/
	void SetHealthBarPercent(float Percent);
	void SetSeconds(int32 InnSeconds);
	void SetMinutes(int32 InnMinutes);
	void EnableGrapplingCrosshair(bool CanGrapple);

private:

	/** Corresponding Widget Elements to WBP */
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Seconds;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Minutes;

	UPROPERTY(meta = (BindWidget))
	class UImage* GrapplingHookCroshair;
};
