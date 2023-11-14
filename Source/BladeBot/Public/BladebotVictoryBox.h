// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BladebotVictoryBox.generated.h"

class UBoxComponent;

UCLASS()
class BLADEBOT_API ABladebotVictoryBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABladebotVictoryBox();

	UFUNCTION(BlueprintCallable)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndexType, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* VictoryBoxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* VictoryBoxCollision;

};
