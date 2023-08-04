// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLADEBOT_API UDeathOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void ExitGame(bool CanExit);

private:
	//UPROPERTY(meta = (BindWidget))
	//	class UImage* GrapplingHookCroshair;

	//UPROPERTY(meta = (BindWidget))
	//	class UButton* ExitButton;
};
