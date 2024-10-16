#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectivePoint.generated.h"

//the tag used for objectives
static FName ObjectiveTag = FName("Object");

UCLASS()
class BLADEBOT_API AObjectivePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AObjectivePoint();

	// Functions -----------------------

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
		void SetInactive();
		UFUNCTION(BlueprintCallable)
		void SetActive();

	// VFX Functions
	UFUNCTION(BlueprintCallable)
		virtual void UpdateVFXLocationRotation();
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXPassive(const FVector& PlayLocation);
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXChange(const FVector& PlayLocation);
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXChangedPassive(const FVector& PlayLocation);

	// Audio functions
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioPassive(const FVector& Location);
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioChange(const FVector& Location);
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioChangedPassive(const FVector& Location);

	// References ------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* ObjectveMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* AfterActivationMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* DisabledMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditAnywhere, Category = "VFX")
		class UNiagaraSystem* VFXPassive;

	UPROPERTY(EditAnywhere, Category = "VFX")
		class UNiagaraSystem* VFXChange;

	UPROPERTY(EditAnywhere, Category = "VFX")
		class UNiagaraSystem* VFXChangedPassive;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundBase* PassiveSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundBase* ChangedSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundBase* ChangedPassiveSound;

	// Constants ------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AlreadyHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ObjectiveComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool shouldBeDestroyed = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DamageToComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int OrderIndex = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RequiresOrder = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isDisabled = false;
};
