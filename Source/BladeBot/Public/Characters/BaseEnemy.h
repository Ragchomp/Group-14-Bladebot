#pragma once

#include "CoreMinimal.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/Character.h"
#include "BaseEnemy.generated.h"

UCLASS()
class BLADEBOT_API ABaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemy();
	virtual void Tick(float DeltaTime) override;



protected:
	virtual void BeginPlay() override;

	// ------------- Functions ------------

	// Health ------------------------------

		// ----
		// Calls recive Damage on attribute component and checks if damage killed self
		// ----
	UFUNCTION(BlueprintCallable)
		virtual void HandleDamage(const float& DamageAmount);

		// ----
		// Removes entity from game and disables collision
		// ----
	UFUNCTION(BlueprintCallable)
		virtual void Die();

	// Sensing ------------------------------

		// ----
		// Returns true if target is in radius from this actor
		// ----
	UFUNCTION(BlueprintCallable)
		virtual bool InTargetRange(const AActor* Target, const float Radius);
		// ----
		// Returns true if location is in radius from this actor
		// ----
		virtual bool InTargetRange(const FVector& Location, const float Radius);

	// VFX ------------------------------

		// ----
		// Spawns a Niagara system at inputs location
		// ----
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXChargeUp(const FVector& PlayLocation);

		// ----
		// Spawns a Niagara system at inputs location
		// ----
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXCoolDown(const FVector& PlayLocation);

		// ----
		// Spawns a Niagara system at inputs location
		// ----
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXAttack(const FVector& PlayLocation);

	// Audio ------------------------------

		// ----
		// Spawns a Sound system at inputs location
		// ----
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioGetHit(const FVector& Location);
	// ----
	// Spawns a Sound system at inputs location
	// ----
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioAttack(const FVector& Location);
	// ----
	// Spawns a Sound system at inputs location
	// ----
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioChargeUp(const FVector& Location);
	// ----
	// Spawns a Sound system at inputs location
	// ----
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioCoolDown(const FVector& Location);

	// Other ------------------------------

		// ----
		// Is called when something starts overlap with this object
		// ----
	UFUNCTION(BlueprintCallable)
		virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

		// ----
		// Is called when something ends overlap with this object
		// ----
	UFUNCTION(BlueprintCallable)
		virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

		// ----
		// A Sphere trace from input startlocation to input endlocation with input radius
		// ----
	UFUNCTION()
		virtual void SphereTrace(FHitResult& OutHit, const FVector& StartLocation, const FVector& EndLocation, const float& traceRadius);



private:	

	// ------------- Components ------------
	UPROPERTY(EditAnywhere)
		class UAttributeComponent* Attributes;

	// Audio ------------------------------
	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundBase* GetHitSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundBase* AttackSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundBase* ChargeUpSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundBase* CoolDownSound;

	// VFX ------------------------------
	UPROPERTY()
		class UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditAnywhere, Category = "VFX")
		class UNiagaraSystem* VFXChargeUp;

	UPROPERTY(EditAnywhere, Category = "VFX")
		class UNiagaraSystem* VFXCoolDown;

	UPROPERTY(EditAnywhere, Category = "VFX")
		class UNiagaraSystem* VFXAttack;
public:

	// ------------- Getters and Setters ------------

	// ----
	// Returns an FVector point, Distance away from a location in the direction of the rotation input
	// ----
	UFUNCTION(BlueprintCallable)
		virtual FVector GetPointWithRotator(const FVector& Start, const FRotator& Rotation, float Distance);

	// ----
	// Returns the forward FVector from an input rotation
	// ----
	UFUNCTION(BlueprintCallable)
		virtual FVector GetVectorOfRotation(const FRotator& Rotation);

	// ----
	// Returns the FVector pointing from point 1, to point 2
	// ----
	UFUNCTION(BlueprintCallable)
		virtual	FVector GetVectorBetweenTwoPoints(const FVector& Point1, const FVector& Point2);

	// ----
	// Returns the distance between two input points as a float
	// ----
	UFUNCTION(BlueprintCallable)
		virtual	float GetDistanceBetweenTwoPoints(const FVector& Point1, const FVector& Point2);

};
