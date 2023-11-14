#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "Blueprint/UserWidget.h"
#include "InputMappingContext.h"
#include "InputMappingWidget.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "SetControlsSettingScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLADEBOT_API USetControlsSettingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	//the current input mapping we're editing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set Controls Setting Screen")
	class UInputMappingContext* CurrentInputMappingContext = nullptr;

	//the default input mapping context to use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set Controls Setting Screen")
	class UInputMappingContext* DefaultInputMappingContext = nullptr;

	//the input action widget class to use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set Controls Setting Screen")
	TSubclassOf<class UInputMappingWidget> InputActionWidgetClass = nullptr;

	//the panel widget to add the input action widgets to
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UPanelWidget* InputActionWidgetsPanel = nullptr;

	//the apply button
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ApplyButton = nullptr;

	//the restore defaults button
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* RestoreDefaultsButton = nullptr;
	
	//array of input action widgets
	UPROPERTY(BlueprintReadOnly, Category = "Set Controls Setting Screen")
	TArray<UInputMappingWidget*> InputActionWidgets = TArray<UInputMappingWidget*>();

	//overrides
	virtual void NativeConstruct() override;

	//called when the apply button is clicked
	UFUNCTION(BlueprintCallable)
	void OnApplyButtonClicked();

	//called when the restore defaults button is clicked
	UFUNCTION(BlueprintCallable)
	void OnRestoreDefaultsButtonClicked();

	//function to add a new input action widget
	UFUNCTION(BlueprintCallable, Category = "Input")
	void AddInputActionWidget(const FEnhancedActionKeyMapping InMapping, TArray<UInputMappingWidget*>& InActionWidgets);

	//function to set the key for an enhanced input action
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetKeyForBinding(UInputMappingWidget* InputActionWidget);

	//function to update an input action widget in an array of input action widgets
	UFUNCTION(BlueprintCallable, Category = "Input")
	void UpdateOrAddInputActionWidget(const int Index, TArray<FEnhancedActionKeyMapping> Mappings, TArray<UInputMappingWidget*>& InActionWidgets);
};
