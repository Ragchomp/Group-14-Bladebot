// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedActionKeyMapping.h"
#include "InputMappingWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLADEBOT_API UInputMappingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//the mapping that is being edited
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Action")
	FEnhancedActionKeyMapping InputMapping = FEnhancedActionKeyMapping();

	//the text block to display the action name
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ActionNameTextBlock = nullptr;

	//the text block to display the action key
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ActionKeyTextBlock = nullptr;

	//the button to remap the action key
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* RemapActionButton = nullptr;

	//whether the widget is currently listening for key presses
	UPROPERTY(BlueprintReadOnly, Category = "Input Action")
	bool bIsListeningForKeyPresses = false;

	//the current key used for this input mapping
	UPROPERTY(BlueprintReadOnly, Category = "Input Action")
	FKey CurrentKey = FKey();

	//overrides
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//called when the remap action button is clicked
	UFUNCTION()
	void OnRemapActionButtonClicked();

	//function to check for possible trigger keys for the input mapping
	UFUNCTION(BlueprintCallable, Category = "Input")
	void CheckForKeyPress(FKey InKey);

	//function to update the ui
	UFUNCTION(BlueprintCallable, Category = "Input")
	void UpdateUI() const;

	//function to set the key for an enhanced input action
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetKey(FKey NewKey);

	//function to set the parameters for the widget
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetMapping(FEnhancedActionKeyMapping InInputMapping);
};
