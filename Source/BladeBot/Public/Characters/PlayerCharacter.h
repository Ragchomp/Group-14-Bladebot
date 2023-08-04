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

	/** Timer Manager  */
	FTimerHandle Seconds;
	void CountSeconds();
	void CableManager();

protected:
	virtual void BeginPlay() override;

	/** Class Components  */
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
		class USpringArmComponent* SpringArm;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UCableComponent* CableComponent;

	/** Subclasses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2-SubObjects")
		TSubclassOf<class AGrapplingHookHead> BP_GrapplingHookHead;

	UPROPERTY()
		class AGrapplingHookHead* GrapplingHookRef{ nullptr };

	/** HUD */
	UPROPERTY()
		class UPlayerOverlay* PlayerOverlay;

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
		void GrappleReel(const FInputActionValue& Value);
	UFUNCTION()
		void Attack(const FInputActionValue& Value);

	/** Bools */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "4-Bools")
	bool IsRetracted = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "4-Bools")
	bool TryingTooReel = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "4-Bools")
	bool InGrappleRange = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4-Bools")
	bool DebugMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4-Bools")
	bool IsMaxTeather = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4-Bools")
	bool TimerShouldTick = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4-Bools")
	bool CanDie = true;

	/** Input Calls */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputMappingContext* IMC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputAction* IA_GroundMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputAction* IA_CameraMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputAction* IA_DoJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputAction* IA_ShootGrapple;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputAction* IA_GrappleReel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
	class UInputAction* IA_Attack;

	/** Constants */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float GrappleMaxDistance = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float PullStrenght = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float CurrentHealth = 3.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "3-Constants")
	float MaxHealth = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float DisplaySeconds = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float DisplayMinutes = 0.f;
	
private:
	UFUNCTION(BlueprintCallable)
	void TokDamage(float DamageAmount);
	UFUNCTION(BlueprintCallable)
	void Die();
	UFUNCTION(BlueprintCallable)
	void TimeManager();
	void GetGrapplingHookRef();
	UFUNCTION(BlueprintCallable)
	void SpawnGrappleProjectile();
	UFUNCTION(BlueprintCallable)
	void GrapplePhysicsUpdate();
	UFUNCTION(BlueprintCallable)
	void GrapplePullUpdate();
	UFUNCTION(BlueprintCallable)
	void DetectIfCanGrapple();
	UFUNCTION(BlueprintCallable)
	void DespawnGrappleIfAtTeatherMax();
	void LineTrace(FHitResult& OutHit);
	UFUNCTION(BlueprintCallable)
	FVector GetPointWithRotator(const FVector& Start, const FRotator& Rotation, float Distance);
	UFUNCTION(BlueprintCallable)
	FVector GetVectorOfRotation(const FRotator& Rotation);
	UFUNCTION(BlueprintCallable)
	FVector GetVectorBetweenTwoPoints(const FVector& Point1, const FVector& Point2);
	UFUNCTION(BlueprintCallable)
	float GetDistanceBetweenTwoPoints(const FVector& Point1, const FVector& Point2);
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent();
	UFUNCTION(BlueprintCallable)
	bool IsAlive();
	void Inits();
	void InputInit();
	void OverlayInit();
	void TimerInit();

	/** State Control  */
	ECharacterState CharacterState = ECharacterState::ECS_Idle;

public:	

	/** Getters & Setters  */
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

};
