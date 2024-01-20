#pragma once

#include "CoreMinimal.h"
#include "StateControl.h"
#include "BaseCharacter.h"
#include "Components/PlayerMovementComponent.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UCameraArmComponent;

//todo fix on start grapple to be called in the movement component when it starts grappling instead of in the player character when it shoots the grapple

UCLASS()
class BLADEBOT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	//constructor with objectinitializer to override the movement component class
	explicit APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	/** Class Components  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UPlayerCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraArmComponent* CameraArm;

	//sphere hitbox for spin attack
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent* SpinAttackHitbox;

	//sword hitbox for spin attack
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* SpinAttackSwordHitbox;

	/** Subclasses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subclasses")
	TSubclassOf<class AGrapplingHookHead> GrappleHookHeadClass;

	UPROPERTY(blueprintReadOnly)
	class AGrapplingHookHead* GrapplingHookRef{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPlayerMovementComponent* PlayerMovementComponent;

	/**
	 * Input Context and Actions
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputMappingContext* IMC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputAction* IA_GroundMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputAction* IA_CameraMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputAction* IA_DoJump;

	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite, Category = "Inputsystem")
	class UInputAction* IA_StopJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputAction* IA_ShootGrapple;

	UPROPERTY(editDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputAction* IA_StopGrapple;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputAction* IA_SpinAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputAction* IA_RespawnButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputAction* IA_RotationToggleOn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inputsystem")
	class UInputAction* IA_RotationToggleOff;

	/** Grapple */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Constants")
	float GrappleSpawnDist = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Constants")
	bool DestroyHookImmediately = false;

	/** Damage */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Constants")
	float Damage = 1.f;

	/** Time manager*/
	UPROPERTY(BlueprintReadOnly, Category = "Time")
	float Seconds = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	float Minutes = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	bool TimerShouldTick = true;

	FTimerHandle TimerHandeler;

	/** Audio */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	FName SpeedParameterName = "Speed";
	
	FTimerHandle RespawnTime;

	//whether or not we can die
	bool CanDie = true;

	/** State Control  */
	ECharacterState CharacterState = ECharacterState::ECS_Idle;

	/**
	 * Spin Attack Variables
	 */
	//how long the spin attack lasts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpinAttack")
	float SpinAttackDuration = 0.5f;

	//how long the spin attack cooldown lasts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpinAttack")
	float SpinAttackCooldownTime = 1.0f;

	//array of actors that have been overlapped by the spin attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpinAttack")
	TArray<AActor*> SpinAttackOverlappedActors;

	//whether or not we're currently doing a spin attack
	UPROPERTY(BlueprintReadOnly, Category = "SpinAttack")
	bool bIsSpinAttacking = false;

	//timer handle for the spin attack
	FTimerHandle SpinAttackTimer = FTimerHandle();

	// Objective Variables
	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	TArray<AActor*> ValidObjectives;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	int NumCompletes = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	int expextedOrder = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	bool GameComplete = false;

	/**
	 * Dash Variables
	 */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	//float DashDuration = 5.0f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	//float DashCooldownTime = 5.0f;

	////dash energy
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	//float DashEnergy;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	//float MaximumDashEnergy = 200.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	//float EnergyRegenerationRate = 1.f;

	////dash movement
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	//float DashSpeed = 5000.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	//float AfterDashSpeed = 1000.f;

	//// Determines if the character can dash
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DashAttack")
	//bool bIsDashing = false;


	////SlowdownSounds
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack")
	//TObjectPtr<USoundBase> DashSound;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack | Effect")
	//TObjectPtr<UNiagaraSystem> DashEffect = nullptr;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack | Effect")
	//float EffectXLocation = 0.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack | Effect")
	//float EffectYLocation = 0.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack | Effect")
	//float EffectPitch = 0.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack | Effect")
	//float EffectYaw = 0.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack | Effect")
	//float EffectRoll = 0.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashAttack | Effect")
	//float EffectZLocation = 0.f;

	//overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void Destroyed() override;
	virtual void Die() override;
	virtual void StopJumping() override;
	void CountTime();
	UFUNCTION()
	void CheckIfObjectivesComplete(AObjectivePoint* Objective);
	UFUNCTION(BlueprintImplementableEvent, Category = "Grappling")
		void ObjectiveComplete();

	void TimerInit();
	void InputInit();
	void Inits();

	/** Respawning Player **/
	UFUNCTION(BlueprintCallable)
	void CallRestartPlayer();

	//getting the character state
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	/**
	 * Grapple Functions
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnGrappleProjectile();

	UFUNCTION()
	void ShootGrapple(const FInputActionValue& Value);

	UFUNCTION()
	void StopGrapple(const FInputActionValue& Value);

	/**
	 * Movement Functions
	*/
	UFUNCTION()
	void GroundMovement(const FInputActionValue& Value);

	UFUNCTION()
	void CameraMovement(const FInputActionValue& Value);

	/**
	 * Jumping Functions
	 */
	UFUNCTION()
	void DoJump(const FInputActionValue& Value);

	UFUNCTION()
	void StopJumpInput(const FInputActionValue& Value);

	/**
	 * Rotation Functions
	 */
	UFUNCTION()
	void RotationToggleOn(const FInputActionValue& Value);

	UFUNCTION()
	void RotationToggleOff(const FInputActionValue& Value);


	/**
	 * Spin Attack Function(s)
	 */

	UFUNCTION()
	void DoSpinAttack(const FInputActionValue& Value);

	UFUNCTION()
	void SpinAttackStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void SpinAttackEnd();

	/**
	 * Dash Function(s)
	 */
	//UFUNCTION()
	//void PlayerDashAttack(const FInputActionValue& Value);

	//UFUNCTION()
	//void ResetCooldownDashOne();

	//UFUNCTION()
	//void EnergyRegeneration();

	//UFUNCTION()
	//void StopDashing();
	

	/**
	 * Blueprint Events
	 */

	//blueprint event for when the player shoots the grappling hook
	UFUNCTION(BlueprintImplementableEvent, Category = "Grappling")
	void OnShootGrapple();

	//blueprint event for when the player starts grappling
	UFUNCTION(BlueprintImplementableEvent, Category = "Grappling")
	void OnStartGrapple();

	//blueprint event for when the player stops grappling
	UFUNCTION(BlueprintImplementableEvent, Category = "Grappling")
	void OnStopGrapple(FVector GrappleHookLocation, bool HasHitWall);

	//blueprint event for when the player does a normal jump
	UFUNCTION(BlueprintImplementableEvent, Category = "Jumping")
	void OnNormalJumpStart();

	//blueprint event for when the player stops jumping a normal jump
	UFUNCTION(BlueprintImplementableEvent, Category = "Jumping")
	void OnJumpStop();

	//blueprint event for when the player does a directional jump
	UFUNCTION(BlueprintImplementableEvent, Category = "Jumping")
	void OnDirectionalJumpStart(FVector Direction);

	//blueprint event for when the player does a corrected directional jump
	UFUNCTION(BlueprintImplementableEvent, Category = "Jumping")
	void OnCorrectedDirectionalJump(FVector OriginalDirection, FVector CorrectedDirection);

	//blueprint event for when the player does a spin attack
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacking")
	void OnSpinAttackBegin();

	//blueprint event for when the spin attack hits something (todo: prvent hitting the same thing twice)
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacking")
	void OnSpinAttackHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//blueprint event for when the spin attack ends
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacking")
	void OnSpinAttackEnd();

	//blueprint event for when the player dies
	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void OnDeath();

	//blueprint event for when the player respawns
	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void OnRespawn();
};
