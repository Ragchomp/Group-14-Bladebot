// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InputMappingWidget.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedActionKeyMapping.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UInputMappingWidget::NativeConstruct()
{
	//call the parent implementation
	Super::NativeConstruct();

	//check if the mapping's input action are valid
	if (InputMapping.Action)
	{
		//set the current key
		CurrentKey = InputMapping.Key;

		//update the ui
		UpdateUI();

		//set the remap button on clicked event
		RemapActionButton->OnClicked.AddDynamic(this, &UInputMappingWidget::OnRemapActionButtonClicked);
	}
}



FReply UInputMappingWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	//check for possible trigger events for the input mapping
	CheckForKeyPress(InKeyEvent.GetKey());

	//return the parent implementation
	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

FReply UInputMappingWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//check for possible trigger events for the input mapping
	CheckForKeyPress(InMouseEvent.GetEffectingButton());

	//return the parent implementation
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UInputMappingWidget::OnRemapActionButtonClicked()
{
	//start listening for any new key press
	bIsListeningForKeyPresses = true;
}

void UInputMappingWidget::CheckForKeyPress(const FKey InKey)
{
	//check if the widget is listening for key presses
	if (bIsListeningForKeyPresses)
	{
		//set the key for the enhanced input action
		SetKey(InKey);

		////set the key text
		//ActionKeyTextBlock->SetText(FText::FromString(InKey.ToString()));

		//update the ui
		UpdateUI();

		//stop listening for key presses
		bIsListeningForKeyPresses = false;
	}
}

void UInputMappingWidget::UpdateUI() const
{
	//set the action name
	ActionNameTextBlock->SetText(FText::FromString(InputMapping.Action->GetName()));

	//set the key text
	ActionKeyTextBlock->SetText(FText::FromString(CurrentKey.ToString()));
}

void UInputMappingWidget::SetKey(const FKey NewKey)
{
	//set the key for the enhanced input action
	CurrentKey = NewKey;
}

void UInputMappingWidget::SetMapping(FEnhancedActionKeyMapping InInputMapping)
{
	//check if the input mapping is valid
	if (InInputMapping == nullptr || !InInputMapping.Action || !InInputMapping.Key.IsValid())
	{
		//skip this input mapping
		return;
	}

	//set the input mapping
	InputMapping = InInputMapping;

	//set the current key
	SetKey(InputMapping.Key);

	//update the ui
	UpdateUI();
}
