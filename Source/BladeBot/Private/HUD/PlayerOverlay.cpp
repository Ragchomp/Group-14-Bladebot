//Main
#include "HUD/PlayerOverlay.h"

//Components
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPlayerOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetSeconds(int32 InnSeconds)
{
	if (Seconds)
	{
		const FString String = FString::Printf(TEXT("%d"), InnSeconds);
		const FText Text = FText::FromString(String);
		Seconds->SetText(Text);
	}
}

void UPlayerOverlay::SetMinutes(int32 InnMinutes)
{
	if (Minutes)
	{
		const FString String = FString::Printf(TEXT("%d"), InnMinutes);
		const FText Text = FText::FromString(String);
		Minutes->SetText(Text);
	}
}

void UPlayerOverlay::EnableGrapplingCrosshair(bool CanGrapple)
{
	if (GrapplingHookCroshair)
	{
		ESlateVisibility CanSee = ESlateVisibility::Hidden;

		if (CanGrapple == true)
		{
			CanSee = ESlateVisibility::Visible;
		}
		else
		{
			CanSee = ESlateVisibility::Hidden;
		}

		GrapplingHookCroshair->SetVisibility(CanSee);
	}
}