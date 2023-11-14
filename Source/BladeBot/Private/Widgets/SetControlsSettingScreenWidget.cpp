#include "Widgets/SetControlsSettingScreenWidget.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Components/PanelWidget.h"
#include "Widgets/InputMappingWidget.h"

void USetControlsSettingScreenWidget::NativeConstruct()
{
    //call the parent implementation
    Super::NativeConstruct();

    //check if the current input mapping context is empty
    if (CurrentInputMappingContext == nullptr || CurrentInputMappingContext->GetMappings().Num() == 0)
    {
        //set the current input mapping context to the default input mapping context
        CurrentInputMappingContext = DefaultInputMappingContext;
    }

	//copy of the mappings
	TArray<FEnhancedActionKeyMapping> Mappings;

	//iterate over all the mapping to sort them by their actions
	for (FEnhancedActionKeyMapping Mapping : CurrentInputMappingContext->GetMappings())
	{
		//check that the array doesn't already contain the mapping
		if (!Mappings.Contains(Mapping))
		{
			//add the mapping to the array
			Mappings.Add(Mapping);

			//get the mapping's action
			const UInputAction* Action = Mapping.Action.Get();

			//iterate over all the mappings again
			for (FEnhancedActionKeyMapping Mapping2 : CurrentInputMappingContext->GetMappings())
			{
				//check if the actions are the same but the keys are different
				if (Action == Mapping2.Action.Get() && Mapping.Key != Mapping2.Key)
				{
					//check that that it's not already in the array
					if (!Mappings.Contains(Mapping2))
					{
						//add the mapping to the array
						Mappings.Add(Mapping2);
					}
				}
			}
		}
	}

    //loop over every input mapping in the current input mapping context
    for (int Index = 0; Index < Mappings.Num(); ++Index)
    {
	     //create a UInputMappingWidget and set its parameters
        AddInputActionWidget(Mappings[Index], InputActionWidgets);
    }

	//bind the buttons on clicked events
	ApplyButton->OnClicked.AddDynamic(this, &USetControlsSettingScreenWidget::OnApplyButtonClicked);
	RestoreDefaultsButton->OnClicked.AddDynamic(this, &USetControlsSettingScreenWidget::OnRestoreDefaultsButtonClicked);
}

void USetControlsSettingScreenWidget::OnApplyButtonClicked()
{
	//iterate through all the input action widgets and set the input mappings
	for (int Index = 0; Index < InputActionWidgets.Num(); ++Index)
	{
		//set the key for the input action widget
		SetKeyForBinding(InputActionWidgets[Index]);
	}

	//get the input subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());

	//update the input subsystem
	Subsystem->RemoveMappingContext(CurrentInputMappingContext);
	Subsystem->AddMappingContext(CurrentInputMappingContext, 0);
	
}

void USetControlsSettingScreenWidget::OnRestoreDefaultsButtonClicked()
{
	//reset the current input mapping context
	CurrentInputMappingContext = DefaultInputMappingContext;

	//loop through all the input mappings and set the input action widgets
	for (int Index = 0; Index < CurrentInputMappingContext->GetMappings().Num(); ++Index)
	{
		//update or add the input action widget
		UpdateOrAddInputActionWidget(Index, CurrentInputMappingContext->GetMappings(), InputActionWidgets);
	}

	//get the input subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayer()->GetLocalPlayer());

	//update the input subsystem
	Subsystem->RemoveMappingContext(CurrentInputMappingContext);
	Subsystem->AddMappingContext(CurrentInputMappingContext, 0);
}

void USetControlsSettingScreenWidget::AddInputActionWidget(const FEnhancedActionKeyMapping InMapping, TArray<UInputMappingWidget*>& InActionWidgets)
{
	//create a UInputMappingWidget and set its parameters
	UInputMappingWidget* InputActionWidget = CreateWidget<UInputMappingWidget>(GetWorld(), InputActionWidgetClass);
	InputActionWidget->InputMapping = InMapping;

	//add the input action widget to the array and the panel
	InActionWidgets.Add(InputActionWidget);
	InputActionWidgetsPanel->AddChild(InputActionWidget);
}

void USetControlsSettingScreenWidget::SetKeyForBinding(UInputMappingWidget* InputActionWidget)
{
	//check if the action and the input mapping context are valid
	if (InputActionWidget->InputMapping.Action && CurrentInputMappingContext)
	{
		//store the action to remap
		const UInputAction* ActionToRemap = InputActionWidget->InputMapping.Action.Get();

		//store the triggers for the action
		TArray<UInputTrigger*> Triggers;
		for (int Index = 0; Index < InputActionWidget->InputMapping.Triggers.Num(); ++Index)
		{
			Triggers.Add(InputActionWidget->InputMapping.Triggers[Index].Get());
		}

		//store the modifiers for the action
		TArray<UInputModifier*> Modifiers;
		for (int Index = 0; Index < InputActionWidget->InputMapping.Modifiers.Num(); ++Index)
		{
			Modifiers.Add(InputActionWidget->InputMapping.Modifiers[Index].Get());
		}

		//remove the old mapping
		CurrentInputMappingContext->UnmapKey(InputActionWidget->InputMapping.Action, InputActionWidget->InputMapping.Key);

		//add the new mapping
		FEnhancedActionKeyMapping& InputMapping = CurrentInputMappingContext->MapKey(ActionToRemap, InputActionWidget->CurrentKey);

		//add back the triggers
		for (int Index = 0; Index < Triggers.Num(); ++Index)
		{
			InputMapping.Triggers.Add(Triggers[Index]);
		}

		//add back the modifiers
		for (int Index = 0; Index < Modifiers.Num(); ++Index)
		{
			InputMapping.Modifiers.Add(Modifiers[Index]);
		}

		//set the input mapping for the input action widget
		InputActionWidget->InputMapping = InputMapping;

		//update the ui
		InputActionWidget->UpdateUI();
	}
}

void USetControlsSettingScreenWidget::UpdateOrAddInputActionWidget(const int Index, TArray<FEnhancedActionKeyMapping> Mappings, TArray<UInputMappingWidget*>& InActionWidgets)
{
	//check if we have an input action widget for this input mapping
	if (InActionWidgets.Num() <= Index)
	{
		AddInputActionWidget(Mappings[Index], InActionWidgets);
	}
	else
	{
		//set the input action widget's parameters
		InActionWidgets[Index]->SetMapping(Mappings[Index]);
	}

	//set the key for the enhanced input action
	SetKeyForBinding(InActionWidgets[Index]);
}
