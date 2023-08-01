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

	/** Class Components  */
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
		class USpringArmComponent* SpringArm;

	/** Subclasses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2 - Sub Objects")
		TSubclassOf<class AGrapplingHookHead> BP_GrapplingHookHead;

	UPROPERTY()
		class AGrapplingHookHead* GrapplingHookRef{ nullptr };

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
	bool IsRetracted = true;
	bool TryingTooReel = false;

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
	float GrappleDistance = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3-Constants")
	float PullStrenght = 3000.f;

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float CurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "3-Constants")
	float MaxHealth = 100.f;
	
private:

	void InputInit();
	void InitOverlay();
	void LineTrace(FHitResult& OutHit);
	void SpawnGrappleProjectile();
	void GetGrapplingHookRef();
	void GrapplePhysicsUpdate();
	void GrapplePullUpdate();
	void DespawnGrappleIfAtTeatherMax();
	void TakeDamage(float DamageAmount);
	void Die();
	

	FVector GetPointWithRotator(const FVector& Start, const FRotator& Rotation, float Distance);
	FVector GetVectorOfRotation(const FRotator& Rotation);
	FVector GetVectorBetweenTwoPoints(const FVector& Point1, const FVector& Point2);
	float GetDistanceBetweenTwoPoints(const FVector& Point1, const FVector& Point2);
	float GetHealthPercent();
	bool IsAlive();

	/** State Control  */
	ECharacterState CharacterState = ECharacterState::ECS_Idle;

public:	

	/** Getters & Setters  */
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

};
