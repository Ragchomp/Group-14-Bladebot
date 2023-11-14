// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BladebotKillBox.generated.h"

class UBoxComponent;

UCLASS()
class BLADEBOT_API ABladebotKillBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABladebotKillBox();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* KillBoxMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* KillBoxCollision;

	UFUNCTION(BlueprintCallable)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndexType, bool bFromSweep, const FHitResult& SweepResult);
protected:

};
