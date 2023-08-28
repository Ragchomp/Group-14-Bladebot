// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckpointLocation.h"

// Sets default values
ACheckpointLocation::ACheckpointLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACheckpointLocation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckpointLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

