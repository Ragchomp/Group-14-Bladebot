#pragma once

#include "CoreMinimal.h"
#include "StateControl.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class BLADEBOT_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

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

		class UCableComponent* CableComponent;

	/** Subclasses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2-SubObjects")
		TSubclassOf<class AGrapplingHookHead> BP_GrapplingHookHead;

	UPROPERTY()
		class AGrapplingHookHead* GrapplingHookRef{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UAttributeComponent* Attributes;

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
	float DisplaySeconds = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float DisplayMinutes = 0.f;
	
private:

	virtual void Die() override;

	virtual void LineTrace(FHitResult& OutHit) override;

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
