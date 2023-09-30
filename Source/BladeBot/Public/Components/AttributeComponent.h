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
		float CurrentHealth = 1;
		
	UPROPERTY(EditAnywhere, Category = "3-Constants")
		float MaxHealth = 1;

public: // Getters and setters
	
	void ReceiveDamage(float damage);
	float GetHealthPercent();
	bool IsNotAlive();
	void SetMaxHealth(const float& h);
	void SetCurrentHealth(const float& h);
};
