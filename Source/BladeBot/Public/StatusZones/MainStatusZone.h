
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainStatusZone.generated.h"

UCLASS()
class BLADEBOT_API AMainStatusZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AMainStatusZone();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// VFX Functions
	UFUNCTION(BlueprintCallable)
	virtual void UpdateVFXLocationRotation();

	UFUNCTION(BlueprintCallable)
	virtual void PlayVFXPassive(const FVector& PlayLocation);

	// Audio Functions
	UFUNCTION(BlueprintCallable)
	virtual void PlayAudioPassive(const FVector& Location);

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* PassiveVFX;

	UPROPERTY(EditAnywhere, Category = "Audio")
	class USoundBase* PassiveSound;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* NiagaraComp;

	// Constants ------------------------------

	bool alreadyHit = false;

};
