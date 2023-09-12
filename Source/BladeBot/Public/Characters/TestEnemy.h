#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "StateControl.h"
#include "TestEnemy.generated.h"

UCLASS()
class BLADEBOT_API ATestEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ATestEnemy();
	virtual void Tick(float DeltaTime) override;

	// ------------ States ---------------------
	EEnemyState EnemyState = EEnemyState::EES_Idle;

protected:
	virtual void BeginPlay() override;



private:	

	// ------------- class Refs ------------
	UPROPERTY()
		class AAIController* EnemyController;

public:
	// ------------- Getters and Setters ------------

		// Gets the AI state from the state controller
	FORCEINLINE EEnemyState GetAIState() const { return EnemyState; }
};
