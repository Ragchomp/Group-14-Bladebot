#pragma once

#include "CoreMinimal.h"
#include "StateControl.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class BLADEBOT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	/** Movement Functions */
	UFUNCTION()
		void GroundMovement(const FInputActionValue& Value);

	UFUNCTION()
		void CameraMovement(const FInputActionValue& Value);

	UFUNCTION()
		void DoJump(const FInputActionValue& Value);


	/** Input Calls */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputMappingContext* IMC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputAction* IA_GroundMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
		class UInputAction* IA_CameraMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputAction* IA_DoJump;



private:

	/** Class Components  */
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
		class USpringArmComponent* SpringArm;


	/** State Control  */
	ECharacterState CharacterState = ECharacterState::ECS_Idle;

public:	

	/** Getters & Setters  */
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

};
