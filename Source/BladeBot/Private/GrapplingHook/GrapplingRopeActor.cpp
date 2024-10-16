#include "GrapplingHook/GrapplingRopeActor.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/PlayerMovementComponent.h"

AGrapplingRopeActor::AGrapplingRopeActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//initialize the collision points array
	RopePoints.Init(FVector(), 2);

	//add the no grapple tag
	Tags.Add(NoGrappleTag);
}

void AGrapplingRopeActor::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//check if the owner and instigator are valid
	if (Owner && GetInstigator())
	{
		//attach the rope to the owner
		AttachToActor(Owner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		//bind the OnOwnerDestroyed function to the owner's OnDestroyed event
		Owner->OnDestroyed.AddDynamic(this, &AGrapplingRopeActor::OnOwnerDestroyed);

		//check if we should use a socket on the instigator pawn
		if (UseSocketOnInstigator)
		{
			//array of static mesh components for the instigator pawn
			TArray<UMeshComponent*> ActorComponents;

			//fill the array with the static mesh components of the instigator pawn
			GetInstigator()->GetComponents(UMeshComponent::StaticClass(), ActorComponents);

			//check if the array is not empty
			if (ActorComponents.Num() > 0)
			{
				//loop through the array
				for (UMeshComponent* MeshComponent : ActorComponents)
				{
					//check if the mesh component has a socket with the given name
					if (MeshComponent->DoesSocketExist(InstigatorSocketName))
					{
						//set the socket mesh to that mesh component
						InstigatorMesh = MeshComponent;
						break;
					}
				}
			}
		}
	}
	//the owner and/or the instigator are invalid
	else
	{
		//print an error message
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Owner or Instigator is invalid for GrapplingRopeActor"));

		//destroy the rope
		Destroy();
	}
}

void AGrapplingRopeActor::Tick(float DeltaTime)
{
	//call the parent implementation
	Super::Tick(DeltaTime);

	//update the collision points
	CheckCollisionPoints();

	//update first and last points or Hitboxes
	SetAttachedRopePointPositions();

	//render the rope
	RenderRope();
}

void AGrapplingRopeActor::Destroyed()
{
	//destroy all the niagara components
	for (UNiagaraComponent* NiagaraComponent : NiagaraComponents)
	{
		NiagaraComponent->DestroyComponent();
	}

	//call the parent implementation
	Super::Destroyed();
}

FVector AGrapplingRopeActor::GetGrapplePoint(AActor* TravelingActor) const
{
	return RopePoints[1];
}

void AGrapplingRopeActor::CheckCollisionPoints()
{
	//setup collision parameters for traces and sweeps
	FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActor(GetInstigator());
	CollisionParams.AddIgnoredActor(Owner);

	//iterate through all the collision points
	for (int Index = 0; Index < RopePoints.Num() - 1; Index++)
	{
		//check if we're not at the first collision point
		if (Index != 0)
		{
			//set jitter to false
			bUseJitter = false;

			//sweep from the previous collision point to the next collision point
			FHitResult Surrounding;
			GetWorld()->SweepSingleByChannel(Surrounding, RopePoints[Index - 1], RopePoints[Index + 1], FQuat(), ECC_Visibility, FCollisionShape::MakeSphere(RopeRadius), CollisionParams);

			//check if the sweep returned a blocking hit or started inside an object
			if (!Surrounding.bBlockingHit && !Surrounding.bStartPenetrating)
			{
				//remove the collision point from the array
				RopePoints.RemoveAt(Index);

				//check if we need to remove the niagara component for this collision point
				if (NiagaraComponents.IsValidIndex(Index) && NiagaraComponents[Index]->IsValidLowLevelFast())
				{
					//destroy the niagara component
					NiagaraComponents[Index]->DestroyComponent();
					
					//remove the niagara component from the array
					NiagaraComponents.RemoveAt(Index);
				}

				//decrement i so we don't skip the next collision point
				Index--;

				//continue to the next collision point
				continue;
			}
		}

		FHitResult Next;
		//sweep from the current collision point to the next collision point
		GetWorld()->SweepSingleByChannel(Next, RopePoints[Index], RopePoints[Index + 1], FQuat(), ECC_Visibility, FCollisionShape::MakeSphere(RopeRadius), CollisionParams);

		//check for hits
		if (Next.IsValidBlockingHit())
		{
			//if we hit something, add a new collision point at the hit location if we're not too close to the last collision point
			if (FVector::Dist(RopePoints[Index], Next.Location) > MinCollisionPointSpacing)
			{
				//insert the new collision point at the hit location
				RopePoints.Insert(Next.Location + Next.ImpactNormal, Index + 1);
			}
		}
	}
}

void AGrapplingRopeActor::SetAttachedRopePointPositions(const bool FixedLength)
{
	//check if we should use a socket on the instigator pawn
	if (UseSocketOnInstigator)
	{
		//set the start of the rope to the socket location
		RopePoints[0] = InstigatorMesh->GetSocketLocation(InstigatorSocketName);
	}
	else
	{
		//set the start of the rope to the instigator pawn's location
		RopePoints[0] = GetInstigator()->GetActorLocation();
	}

	//set the end of the rope to the owner's location
	RopePoints[RopePoints.Num() - 1] = Owner->GetActorLocation();

	//check if we're using debug rendering mode
	if (bUseDebugDrawing)
	{
		//draw a debug sphere at the socket location
		DrawDebugSphere(GetWorld(), RopePoints[0], 10.f, 12, FColor::Red, false, 0.f);	
	}
}

void AGrapplingRopeActor::SpawnNiagaraSystem(int Index)
{
	//create a new Niagara component
	UNiagaraComponent* NewNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, RopePoints[Index]);

	//set the end location of the Niagara component
	NewNiagaraComponent->SetVectorParameter(RibbonEndParameterName, RopePoints[Index + 1]);

	//set tick group and behavior
	NewNiagaraComponent->SetTickGroup(TickGroup);
	NewNiagaraComponent->SetTickBehavior(TickBehavior);

	//add the no grapple tag to the Niagara component
	NewNiagaraComponent->ComponentTags.Add(NoGrappleTag);

	//add the new Niagara component to the array
	NiagaraComponents.Add(NewNiagaraComponent);
}

void AGrapplingRopeActor::RenderRope()
{
	//chceck if we should use debug drawing
	if (bUseDebugDrawing)
	{
		//iterate through all the collision points except the last one
		for (int Index = 0; Index < RopePoints.Num() - 1; ++Index)
		{
			//draw a debug line between the current collision point and the next collision point
			DrawDebugLine(GetWorld(), RopePoints[Index], RopePoints[Index + 1], FColor::Red, false, 0.f, 0, 5.f);
		}
	}
	//check if we don't have a valid Niagara system to render
	else if (NiagaraSystem->IsValidLowLevelFast())
	{
		//check if we have a valid Niagara system to render
		if (NiagaraSystem->IsValidLowLevelFast())
		{
			//iterate through all the collision points except the last one
			for (int Index = 0; Index < RopePoints.Num() - 1; ++Index)
			{
				//check if we have a valid Niagara component to use or if we need to create a new one
				if (NiagaraComponents.IsValidIndex(Index) && NiagaraComponents[Index]->IsValidLowLevelFast())
				{
					//set the start location of the Niagara component
					NiagaraComponents[Index]->SetWorldLocation(RopePoints[Index]);

					//set the end location of the Niagara component
					NiagaraComponents[Index]->SetVectorParameter(RibbonEndParameterName, RopePoints[Index + 1]);

					//set whether or not to use jitter
					NiagaraComponents[Index]->SetBoolParameter(JitterParameterName, bUseJitter);

					//check if we should use rope radius
					if (UseRopeRadiusAsRibbonWidth)
					{
						//set the ribbon width to the rope radius
						NiagaraComponents[Index]->SetFloatParameter(RibbonWidthParameterName, RopeRadius /* * 2*/);
					}
					else
					{
						//set the ribbon width to the ribbon width
						NiagaraComponents[Index]->SetFloatParameter(RibbonWidthParameterName, RibbonWidth);
					}
				}
				else
				{
					//create a new Niagara component
					SpawnNiagaraSystem(Index);
				}
			}
		}
		else
		{
			//draw debug message
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RopeRenderer has no valid niagara system"));
		}
	}
	else
	{
		//draw debug message
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Draw debug is false, and we don't have a valid niagara system"));
	}
}

float AGrapplingRopeActor::GetRopeLength() const
{
	//initialize the rope length
	float Length = 0.f;

	//iterate through all the collision points except the last one
	for (int Index = 0; Index < RopePoints.Num() - 1; ++Index)
	{
		//add the distance between the current collision point and the next collision point to the rope length
		Length += FVector::Dist(RopePoints[Index], RopePoints[Index + 1]);
	}

	//return the rope length
	return Length;
}

void AGrapplingRopeActor::OnOwnerDestroyed(AActor* DestroyedActor)
{
	//destroy ourselves
	Destroy();
}
