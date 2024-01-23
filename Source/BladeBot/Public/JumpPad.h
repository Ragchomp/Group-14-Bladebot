#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JumpPad.generated.h"

UCLASS()
class BLADEBOT_API AJumpPad : public AActor
{
	GENERATED_BODY()
	
public:	
	AJumpPad();

	// Functions ---------------------------

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void throwPlayer(class APlayerCharacter* Player, FVector ThrowAngle, float throwPower);

	void canJumpAgain();

	UFUNCTION()
		virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// VFX Functions
	UFUNCTION(BlueprintCallable)
		virtual void UpdateVFXLocationRotation();
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXPassive(const FVector& PlayLocation);
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXActivate(const FVector& PlayLocation);

	// Audio Functions
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioPassive(const FVector& Location);
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioActivate(const FVector& Location);

	// References ------------------------------

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		class UCapsuleComponent* CollisionMesh;

	UPROPERTY(EditAnywhere)
		class UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditAnywhere, Category = "VFX")
		class UNiagaraSystem* VFXPassive;

	UPROPERTY(EditAnywhere, Category = "VFX")
		class UNiagaraSystem* VFXActivate;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundBase* PassiveSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundBase* ActivateSound;


	// Handles ------------------------------

	FTimerHandle JumpResetTimerHandeler;

	// Constants ------------------------------

	UPROPERTY(EditAnywhere, Category = "Constant")
	FVector throwAngle = GetActorUpVector();

	UPROPERTY(EditAnywhere, Category = "Constant")
	float throwPower = 400000.0f;

	bool Jumped = false;

	UPROPERTY(EditAnywhere, Category = "Constant")
	float JumppadCooldown = 1.0f;
};
