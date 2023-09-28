#pragma once

#include "CoreMinimal.h"
#include "StateControl.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UCameraArmComponent;

UCLASS()
class BLADEBOT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	//constructor with objectinitializer to override the movement component class
	explicit APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Timer Manager  */
	FTimerHandle Seconds;
	FTimerHandle NoiseTimer;
	void CountSeconds();
	void CableManager();

	void GetGrapplingHookRef();

	UFUNCTION(BlueprintCallable)
	void GrapplePullUpdate();

	UFUNCTION(BlueprintCallable)
	void SpawnGrappleProjectile();

	/** Bools */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GrappleHook|Bools")
	bool IsRetracted = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GrappleHook|Bools")
	bool TryingTooReel = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GrappleHook|Bools")
	bool InGrappleRange = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook|Bools")
	bool DebugMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook|Bools")
	bool IsMaxTeather = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer|Bools")
	bool TimerShouldTick = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools|Others")
	bool CanDie = true;

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

protected:
	virtual void BeginPlay() override;
	/** Input Functions */
	UFUNCTION()
	void GroundMovement(const FInputActionValue& Value);
	UFUNCTION()
	void CameraMovement(const FInputActionValue& Value);
	UFUNCTION()
	void DoJump(const FInputActionValue& Value);
	UFUNCTION()
	void ShootGrapple(const FInputActionValue& Value);
	UFUNCTION()
	void GrappleReel();
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

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Actions")
	//class UInputAction* IA_GrappleReel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Actions")
	class UInputAction* IA_Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Actions")
	class UInputAction* IA_DashAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem|Actions")
	class UInputAction* IA_RespawnButton;

	/** Class Components  */
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	class UCameraArmComponent* CameraArm;

	class UCableComponent* CableComponent;

	/** Subclasses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AGrapplingHookHead> BP_GrapplingHookHead;

	UPROPERTY()
	class AGrapplingHookHead* GrapplingHookRef{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAttributeComponent* Attributes;

	/** HUD */
	UPROPERTY()
	class UPlayerOverlay* PlayerOverlay;

	/** Constants */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook|Constants")
	float GrappleMaxDistance = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook|Constants")
	float PullStrenght = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer|Constants")
	float DisplaySeconds = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer|Constants")
	float DisplayMinutes = 0.f;

	/** Respawning Player **/
	virtual void Destroyed() override;
	//virtual void BeginDestroy() override;
	void CallRestartPlayer();

private:
	virtual void Die() override;

	virtual void LineTrace(FHitResult& OutHit) override;

	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent = GetCharacterMovement();;

	UFUNCTION(BlueprintCallable)
	void TimeManager();

	UFUNCTION(BlueprintCallable)
	void GrapplePhysicsUpdate();

	UFUNCTION(BlueprintCallable)
	void DetectIfCanGrapple();

	UFUNCTION(BlueprintCallable)
	void DespawnGrappleIfAtTeatherMax();

	void Inits();
	void InputInit();
	void OverlayInit();
	void TimerInit();
	void GenerateNoise();

	/** State Control  */
	ECharacterState CharacterState = ECharacterState::ECS_Idle;

public:

	/** Getters & Setters  */
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
