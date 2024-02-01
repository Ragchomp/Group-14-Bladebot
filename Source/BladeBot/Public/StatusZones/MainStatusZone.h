
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainStatusZone.generated.h"

UCLASS()
class BLADEBOT_API AMainStatusZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AMainStatusZone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
