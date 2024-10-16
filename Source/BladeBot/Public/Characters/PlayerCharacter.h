#pragma once

#include "CoreMinimal.h"
#include "StateControl.h"
#include "BaseCharacter.h"
#include "Components/PlayerMovementComponent.h"
#include "PlayerCharacter.generated.h"

class AObjectivePoint;
struct FInputActionValue;
class UCameraArmComponent;

UCLASS()
class BLADEBOT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	//constructor with objectinitializer to override the movement component class
	explicit APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	//whether or not the character can currently jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	bool bCanJump = true;

	//whether or not we're currently in the process of resetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jumping")
	bool bIsResetting = true;

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

	//movement parameters for the spin attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpinAttack")
	FAsyncRootMovementParams SpinAttackMovementParams = FAsyncRootMovementParams(FVector::ZeroVector, 0,0,0.5f,true);

	//the speed we clamp the player's speed to when they do a spin attack and don't hit anything
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpinAttack")
	float SpinAttackPunishmentSpeed = 1000.f;

	//whether or not we've hit an enemy with the current or last spin attack
	UPROPERTY(BlueprintReadOnly, Category = "SpinAttack")
	bool bHitEnemyWithSpinAttack = false;

	//whether or not we're currently doing a spin attack
	UPROPERTY(BlueprintReadOnly, Category = "SpinAttack")
	bool bIsSpinAttacking = false;

	//whether or not we can do a spin attack (excluding whether or not we're currently doing a spin attack)
	UPROPERTY(BlueprintReadOnly, Category = "SpinAttack")
	bool bCanSpinAttack = true;

	//timer handle for the spin attack
	FTimerHandle SpinAttackTimer = FTimerHandle();
	FTimerHandle SpinAttackCooldownTimer = FTimerHandle();

	//array of actors that have been overlapped by the spin attack
	UPROPERTY()
	TArray<AActor*> SpinAttackOverlappedActors;

	// Objective Variables
	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	TArray<AActor*> ValidObjectives;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objective")
	int NumCompletes = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objective")
	int NumEnabledObjectivesTotal;

	UPROPERTY(BlueprintReadWrite, Category = "Objective")
	int expextedOrder = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Objective")
	bool GameComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	int numEnemiesDestroyed = 0;
	

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
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void SetPlayerDefaults() override;


	
	UFUNCTION()
	void CheckIfObjectivesComplete(AObjectivePoint* Objective);
	UFUNCTION(BlueprintImplementableEvent, Category = "Grappling")
	void ObjectiveComplete();

	void InputInit();
	void Inits();

	//function to update the objective and enemy related variables
	UFUNCTION(BlueprintCallable)
	void UpdateObjectiveEnemyVariables();

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
	void SpinAttackEnd();

	UFUNCTION()
	void SpinAttackCooldownEnd();

	UFUNCTION()
	void SpinAttackMovementEnd();

	UFUNCTION()
	void DoSpinAttackOnEnemy(AActor* Enemy);

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
	void OnStartGrapple(FHitResult GrappleHit, EGrappleHitType GrappleHitType);

	//blueprint event for when the player stops grappling
	UFUNCTION(BlueprintImplementableEvent, Category = "Grappling")
	void OnStopGrapple(FVector GrappleHookLocation, bool HasHitWall, bool IsReset);

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

	//blueprint event for when the spin attack hits an enemy (called for each enemy hit)
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacking")
	void OnSpinAttackHit(AActor* HitActor);

	//blueprint event for when the spin attack hits something that isn't an enemy (called for each non-enemy hit)
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacking")
	void OnSpinAttackHitNonEnemy(AActor* HitActor);

	//blueprint event for when the spin attack ends
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacking")
	void OnSpinAttackEnd();

	//blueprint event for when the spin attack cooldown ends
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacking")
	void OnSpinAttackCooldownEnd();

	//blueprint event for when the player starts wall latching
	UFUNCTION(BlueprintImplementableEvent, Category = "Wall Latch")
	void OnWallLatch(FHitResult HitResult);

	//blueprint event for when the player falls off a wall latch
	UFUNCTION(BlueprintImplementableEvent, Category = "Wall Latch")
	void OnWallLatchFall();

	//blueprint event for when the player launches off a wall latch
	UFUNCTION(BlueprintImplementableEvent, Category = "Wall Latch")
	void OnWallLatchLaunch();

	//blueprint event for when the player starts wall running
	UFUNCTION(BlueprintImplementableEvent, Category = "Wall Run")
	void OnWallRunStart(FHitResult HitResult);

	//blueprint event for when the player does a wall run jump
	UFUNCTION(BlueprintImplementableEvent, Category = "Wall Run")
	void OnWallRunJump();

	//blueprint event for when the player stops wall running
	UFUNCTION(BlueprintImplementableEvent, Category = "Wall Run")
	void OnWallRunFinish();

	//blueprint event for when the player dies
	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void OnDeath();

	//blueprint event for when the player respawns
	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void OnRespawn();


};
