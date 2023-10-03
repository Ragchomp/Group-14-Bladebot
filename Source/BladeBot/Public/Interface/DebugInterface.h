//copyright here

#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/Engine.h"
#include "DebugInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UDebugInterface : public UInterface
{
	GENERATED_BODY()
};

class IDebugInterface
{
	GENERATED_BODY()

protected:
	bool bDebugMode = false;

	virtual bool ShouldPrint() { return GEngine && bDebugMode; }
	virtual void DefaultDebugPrint(UObject* Object, FString Message = FString(), FColor Color = FColor::Red, float TimeToDisplay = 5.f);
	virtual void DebugPrint(UObject* Object, int Key, float TimeToDisplay, FColor Color, FString Message = FString()); 
};

inline void IDebugInterface::DefaultDebugPrint(UObject* Object, FString Message, FColor Color, float TimeToDisplay)
{
	DebugPrint(Object, -1, TimeToDisplay, Color, Message);
}

inline void IDebugInterface::DebugPrint(UObject* Object, int Key, float TimeToDisplay, FColor Color, FString Message)
{
	//check if we have a valid engine and if we are in debug mode
	if (ShouldPrint())
	{
		//if we don't have a message, set it to the default
		if (Message.IsEmpty())
		{
			//set the message to the default
			Message = "DefaultDebugPrint() called from " + Object->GetClass()->GetName();
		}
		//print the message
		GEngine->AddOnScreenDebugMessage(Key, TimeToDisplay, Color, Message);
	}
}
