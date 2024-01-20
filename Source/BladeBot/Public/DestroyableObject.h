#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestroyableObject.generated.h"

UCLASS()
class BLADEBOT_API ADestroyableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	ADestroyableObject();

	// Functions ------------------------------
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		virtual void ChangeMesh();

	// VFX Functions
	UFUNCTION(BlueprintCallable)
		virtual void UpdateVFXLocationRotation();
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXPassive(const FVector& PlayLocation);
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXChange(const FVector& PlayLocation);
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXChangedPassive(const FVector& PlayLocation);

	// Audio Functions
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioPassive(const FVector& Location);
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioChange(const FVector& Location);
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioChangedPassive(const FVector& Location);

	// References ------------------------------

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

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* ActiveMesh;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* ChangedMesh;

	UPROPERTY(EditAnywhere)
		class USphereComponent* CollisionMesh;

	UPROPERTY(EditAnywhere)
		class UNiagaraComponent* NiagaraComp;

	// Constants ------------------------------

	bool alreadyHit = false;
};

