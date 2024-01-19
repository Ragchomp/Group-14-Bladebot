// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectivePoint.generated.h"

UCLASS()
class BLADEBOT_API AObjectivePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectivePoint();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
		virtual void UpdateVFXLocationRotation();
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXPassive(const FVector& PlayLocation);
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXChange(const FVector& PlayLocation);
	UFUNCTION(BlueprintCallable)
		virtual void PlayVFXChangedPassive(const FVector& PlayLocation);

	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioPassive(const FVector& Location);
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioChange(const FVector& Location);
	UFUNCTION(BlueprintCallable)
		virtual void PlayAudioChangedPassive(const FVector& Location);

	void getOrderIndex();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ObjectveMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* AfterActivationMesh;

	UPROPERTY(EditAnywhere)
	class USphereComponent* CollisionMesh;

	UPROPERTY(EditAnywhere)
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

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool AlreadyHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ObjectiveComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int OrderIndex = 1;


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
