// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/HitInterface.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class BLADEBOT_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	explicit ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	virtual void Die();
	virtual void LineTrace(FHitResult& OutHit);

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		virtual FVector GetPointWithRotator(const FVector& Start, const FRotator& Rotation, float Distance);

	UFUNCTION(BlueprintCallable)
		virtual FVector GetVectorOfRotation(const FRotator& Rotation);

	UFUNCTION(BlueprintCallable)
		virtual	FVector GetVectorBetweenTwoPoints(const FVector& Point1, const FVector& Point2);

	UFUNCTION(BlueprintCallable)
		virtual	float GetDistanceBetweenTwoPoints(const FVector& Point1, const FVector& Point2);

protected:
	virtual void BeginPlay() override;


};
