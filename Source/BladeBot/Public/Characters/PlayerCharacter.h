#pragma once

#include "CoreMinimal.h"
#include "StateControl.h"
#include "BaseCharacter.h"
#include "Components/PlayerMovementComponent.h"
#include "Interface/DebugInterface.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UCameraArmComponent;

UCLASS()
class BLADEBOT_API APlayerCharacter : public ABaseCharacter, public IDebugInterface
{
	GENERATED_BODY()

public:
	//constructor with objectinitializer to override the movement component class
	explicit APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	//overrides
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FTimerHandle Seconds;
	void CountTime();

	UFUNCTION(BlueprintCallable)
	void SpawnGrappleProjectile();
	void TimerInit();
	void InputInit();
	void Inits();

	/** Bools */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer|Bools")
	bool TimerShouldTick = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools|Others")
	bool CanDie = true;


protected:
	virtual void BeginPlay() override;
	/** Input Functions */

	//function to check if the player can use input
	UFUNCTION(blueprintCallable)
	bool CanUseInput(const FInputActionValue& Value);

	//function for input debug messages
	void InputDebugMessage(const class UInputAction* InputAction, const FString& DebugMessage = "Input Received");

	UFUNCTION()
	void GroundMovement(const FInputActionValue& Value);

	UFUNCTION()
	void CameraMovement(const FInputActionValue& Value);

	UFUNCTION()
	void DoJump(const FInputActionValue& Value);

	UFUNCTION()
	void ShootGrapple(const FInputActionValue& Value);

	UFUNCTION()
	void DespawnGrapple(const FInputActionValue& Value);

	//UFUNCTION()
	//void GrappleReel(const FInputActionValue& Value);

	//UFUNCTION()
	//void StopGrappleReel(const FInputActionValue& Value);

	UFUNCTION()
	void Attack(const FInputActionValue& Value);

	/** Input Calls */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|IMC")
	class UInputMappingContext* IMC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Movement")
	class UInputAction* IA_GroundMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Movement")
	class UInputAction* IA_CameraMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Movement")
	class UInputAction* IA_DoJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Actions")
	class UInputAction* IA_ShootGrapple;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Actions")
	class UInputAction* IA_DespawnGrapple;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Actions")
	class UInputAction* IA_Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Actions")
	class UInputAction* IA_DashAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Actions")
	class UInputAction* IA_RespawnButton;

	/**
	 * Dash Function
	 */
	UFUNCTION()
	void PlayerDashAttack(const FInputActionValue& Value);

	////SlowdownSounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	TObjectPtr<USoundBase> DashSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	float DashSpeed = 1.f;

	void DashCheck();

public:

	/** Class Components  */
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	class UCameraArmComponent* CameraArm;

	/** Subclasses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AGrapplingHookHead> GrappleHookHeadClass;

	UPROPERTY()
	class AGrapplingHookHead* GrapplingHookRef{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAttributeComponent* Attributes;

	UPROPERTY()
	UPlayerMovementComponent* PlayerMovementComponent;

	/** HUD */
	UPROPERTY()
	class UPlayerOverlay* PlayerOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer|Constants")
	float DisplaySeconds = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer|Constants")
	float DisplayMinutes = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Constants")
	float GrappleSpawnDist = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Constants")
	float GrappleSpeed = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer|Constants")
	float MovementSpeedToKill = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer|Constants")
	float Damage = 1.f;

	/** Respawning Player **/
	virtual void Destroyed() override;
	//virtual void BeginDestroy() override;
	void CallRestartPlayer();

private:
	virtual void Die() override;
	void OverlayInit();
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** State Control  */
	ECharacterState CharacterState = ECharacterState::ECS_Idle;

public:

	/** Getters & Setters  */
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
