#include "GrapplingHook/GrapplingRopeActor.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AGrapplingRopeActor::AGrapplingRopeActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//initialize the collision points array
	RopePoints.Init(FVector(), 2);
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

	//update first and last points or Hitboxes
	SetAttachedRopePointPositions();

	//check if we're in the infinite rope length mode
	if (RopeMode == InfiniteRopeLength)
	{
		CheckCollisionPoints();
	}

	//check if we should use debug draw
	if (bDrawDebugRope)
	{
		//draw the rope
		DrawDebugRope();
	}
	//check if we don't have a valid Niagara system to render
	else if (NiagaraSystem->IsValidLowLevelFast())
	{
		//render the rope
		RenderRope();
	}
	else
	{
		//draw debug message
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Draw debug is false, and we don't have a valid niagara system"));
	}
}

void AGrapplingRopeActor::Destroyed()
{
	//destroy all the physics constraints
	for (UPhysicsConstraintComponent* PhysicsConstraint : PhysicsConstraints)
	{
		PhysicsConstraint->DestroyComponent();
	}

	//destroy all the hitboxes
	for (USphereComponent* Hitbox : Hitboxes)
	{
		Hitbox->DestroyComponent();
	}

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
	CollisionParams.AddIgnoredActor(GetInstigator());
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
			if (!Surrounding.bBlockingHit || Surrounding.bStartPenetrating)
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
	//check if we're in the infinite rope length mode
	if (FixedLength)
	{
		//set the first hitbox's location to the instigator pawn's location
		Hitboxes[0]->SetWorldLocation(GetInstigator()->GetActorLocation());
	}
	else
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
	}
}

void AGrapplingRopeActor::RenderRope()
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
				UNiagaraComponent* NewNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, RopePoints[Index]);

				//set the end location of the Niagara component
				NewNiagaraComponent->SetVectorParameter(RibbonEndParameterName, RopePoints[Index + 1]);

				//add the new Niagara component to the array
				NiagaraComponents.Add(NewNiagaraComponent);
			}
		}
	}
	else
	{
		//draw debug message
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RopeRenderer has no valid niagara system"));
	}
}

void AGrapplingRopeActor::DrawDebugRope()
{
	//check which rope mode we're in
	switch (RopeMode)
	{
		case InfiniteRopeLength:
		{
			//draw all the parts of the rope in between
			for (int i = 0; i < RopePoints.Num() - 1; i++)
			{
				DrawDebugLine(GetWorld(), RopePoints[i], RopePoints[i + 1], FColor::Red, false, 0, 0, 3);
			}
			break;
		}
		case FixedLength:
		{
			//draw all the hitboxes
			for (int i = 0; i < Hitboxes.Num() - 1; i++)
			{
				DrawDebugLine(GetWorld(), Hitboxes[i]->GetComponentLocation(), Hitboxes[i + 1]->GetComponentLocation(), FColor::Red, false, 0, 0, 3);
			}

			//check if we should draw the physics constraints
			if (bDrawPhysicsConstraints)
			{
				//draw all the physics constraints
				for (int i = 0; i < PhysicsConstraints.Num(); i++)
				{
					DrawDebugLine(GetWorld(), PhysicsConstraints[i]->GetComponentLocation(), PhysicsConstraints[i]->GetComponentLocation(), FColor::Red, false, 0, 0, 3);
				}
			}

			break;
		}
		default: ;
	}
}

void AGrapplingRopeActor::SetRopeMode(const ERopeMode NewRopeMode)
{
	if (NewRopeMode == RopeMode)
	{
		return;
	}
	if (NewRopeMode == InfiniteRopeLength)
	{
	}
	//needs to deal with corners better
	else
	{
		//check if we have any collision points to convert
		if (RopePoints.IsEmpty())
		{
			return;
		}

		//fill the space between collision points with Hitboxes and physics constraints
		for (int Index = 0; Index < RopePoints.Num(); ++Index)
		{
			//don't process the last collision point
			if (!RopePoints.IsValidIndex(Index + 1))
			{
				break;
			}

			//Get start and end locations
			FVector StartLocation = RopePoints[Index];
			FVector EndLocation = RopePoints[Index + 1];

			//Get the direction and distance between the 2 locations
			FVector Direction = (EndLocation - StartLocation).GetSafeNormal();
			const float Distance = FVector::Dist(StartLocation, EndLocation);

			//SpawnDistance equal to RopeRadius * 3 (2 for the previous sphere and 1 for half of the current sphere) + HitboxSpacing
			const float SpawnDistance = HitboxSpacing + 2 * RopeRadius;

			//spawn the hitboxes
			for (int Index2 = 0; Index2 < Distance; Index2++)
			{
				//if the distance between the 2 locations is less than the spawn distance, don't spawn a hitbox instead attach the last hitbox to the end location
				if (Index2 * SpawnDistance > Distance)
				{
					//Spawn a new physics constraint
					UPhysicsConstraintComponent* NewPhysicsConstraint = NewObject<UPhysicsConstraintComponent>(this, UPhysicsConstraintComponent::StaticClass());

					//Set the constraint parameters
					FVector LocDirection = Owner.Get()->GetActorLocation() - EndLocation;
					NewPhysicsConstraint->SetWorldLocation(EndLocation + LocDirection / 2);
					NewPhysicsConstraint->ConstraintActor1 = Owner;
					NewPhysicsConstraint->ConstraintActor2 = this;
					NewPhysicsConstraint->SetDisableCollision(true);
					NewPhysicsConstraint->SetConstrainedComponents(nullptr, NAME_None, Hitboxes.Last(), NAME_None);

					//Add the physics constraint to the array
					PhysicsConstraints.Add(NewPhysicsConstraint);

					//stop spawning hitboxes
					break;
				}

				//Spawn a new hitbox
				USphereComponent* NewSphere = NewObject<USphereComponent>(this, USphereComponent::StaticClass());

				NewSphere->RegisterComponent();
				NewSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				NewSphere->InitSphereRadius(RopeRadius);
				NewSphere->SetVisibility(true);

				//Set Simulate Physics to true
				NewSphere->SetSimulatePhysics(true);
				NewSphere->SetEnableGravity(true);

				//Set the radius of the sphere
				NewSphere->SetSphereRadius(RopeRadius);

				//Set the location of the Hitbox
				NewSphere->SetWorldLocation(StartLocation + Direction * SpawnDistance * Index2);

				//Set Collision Profile of the hitbox
				NewSphere->SetCollisionProfileName("PhysicsActor");
				NewSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				NewSphere->SetCollisionResponseToAllChannels(ECR_Block);
				NewSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
				NewSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

				//Spawn a new physics constraint
				UPhysicsConstraintComponent* NewPhysicsConstraint = NewObject<UPhysicsConstraintComponent>(this, UPhysicsConstraintComponent::StaticClass());

				//if hitboxes is empty add a physics constraint to the attached actor
				if (Hitboxes.IsEmpty())
				{
					//Set the constraint parameters
					NewPhysicsConstraint->SetWorldLocation(StartLocation + Direction * (RopeRadius + HitboxSpacing / 2));
					NewPhysicsConstraint->ConstraintActor1 = GetInstigator();
					NewPhysicsConstraint->ConstraintActor2 = this;
					NewPhysicsConstraint->SetDisableCollision(true);
					NewPhysicsConstraint->SetConstrainedComponents(nullptr, NAME_None, NewSphere, NAME_None);
				}
				else
				{
					//else add a physics constraint to the previous hitbox
					//Get the previous hitbox
					USphereComponent* PreviousSphere = Hitboxes.Last();

					//Set the constraint parameters
					NewPhysicsConstraint->SetWorldLocation(PreviousSphere->GetComponentLocation() + Direction * (RopeRadius + HitboxSpacing / 2));
					NewPhysicsConstraint->SetConstrainedComponents(PreviousSphere, NAME_None, NewSphere, NAME_None);
				}

				NewPhysicsConstraint->ConstraintInstance.ProfileInstance.bEnableProjection = false;

				//Add the physics constraint to the array
				PhysicsConstraints.Add(NewPhysicsConstraint);

				//Add the hitbox to the array
				Hitboxes.Add(NewSphere);
			}
		}

		//Set the mass of the new sphere so it decreases with each sphere spawned in order to stabilize the physics of the rope
		for (int Index = 0; Index < Hitboxes.Num(); ++Index)
		{
			Hitboxes[Index]->SetMassOverrideInKg(NAME_None, 100 / (Index + 1), true); //Mass = 1 / [Index + 1
		}
	}

	//set the rope mode
	RopeMode = NewRopeMode;
}

void AGrapplingRopeActor::OnOwnerDestroyed(AActor* DestroyedActor)
{
	//break all the physics constraints
	for (UPhysicsConstraintComponent* PhysicsConstraint : PhysicsConstraints)
	{
		PhysicsConstraint->BreakConstraint();
	}

	//destroy ourselves
	Destroy();
}
