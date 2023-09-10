#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLADEBOT_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "3-Constants")
		float CurrentHealth;
		
	UPROPERTY(EditAnywhere, Category = "3-Constants")
		float MaxHealth;

public: // Getters and setters
	
	void ReciveDamage(float damage);
	float GetHealthPercent();
	bool IsAlive();
};
