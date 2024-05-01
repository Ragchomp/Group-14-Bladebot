#include "Characters/PlayerCharacter.h"
#include "GrapplingHook/GrapplingHookHead.h"
#include "Components/AttributeComponent.h"
#include "Engine/DamageEvents.h"

//Components
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CameraArmComponent.h"
#include "Components/PlayerMovementComponent.h"
#include "EnhancedInputComponent.h"

//Gameplay Statics
#include "Controllers/BladebotPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include <EnhancedInputSubsystems.h>
#include "BladebotGameMode.h"
#include "EngineUtils.h"
#include "BladeBot/Spawning/SpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Components/PlayerCameraComponent.h"
#include "Components/SphereComponent.h"
#include "Objectives/ObjectivePoint.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UPlayerMovementComponent>(CharacterMovementComponentName))
{
	//Enable ticking
	PrimaryActorTick.bCanEverTick = true;

	//Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//set rotation to follow movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	//get a reference to the movement component as a player movement component
	PlayerMovementComponent = Cast<UPlayerMovementComponent>(GetCharacterMovement());

	//create our components
	CameraArm = CreateDefaultSubobject<UCameraArmComponent>(TEXT("CameraArm"));
	Camera = CreateDefaultSubobject<UPlayerCameraComponent>(TEXT("Camera"));
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	SpinAttackHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("SpinAttackHitbox"));
	SpinAttackSwordHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpinAttackSwordHitbox"));

	//setup attachments
	CameraArm->SetupAttachment(GetRootComponent());
	Camera->SetupAttachment(CameraArm);
	SpinAttackHitbox->SetupAttachment(GetRootComponent());
	SpinAttackSwordHitbox->SetupAttachment(GetRootComponent());

	//set spin attack hitbox collision settings and size
	SpinAttackHitbox->SetSphereRadius(1000);

	//set relative location and rotation for the mesh
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -60.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	//set the mesh's collision profile to be no collision
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	//set the capsule component's default size
	GetCapsuleComponent()->SetCapsuleHalfHeight(60);
	GetCapsuleComponent()->SetCapsuleRadius(10);

	//set the camera arm's target offset to be above the character and a little behind
	CameraArm->TargetOffset = FVector(0.f, 10.f, 90.f);

	//make the camera follow the controller's rotation (so it uses the rotation input from the mouse)
	CameraArm->bUsePawnControlRotation = true;

	//disable busepawncontrolrotation on the camera
	Camera->bUsePawnControlRotation = false;

	//default to automatically possessing the player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//add the player tag
	Tags.Add(FName("Player"));

	////DashInit
	//MaximumDashEnergy = 200.f;
	//EnergyRegenerationRate = 1;
	//DashEnergy = MaximumDashEnergy;
}

float APlayerCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	//check if we have a valid attribute component
	if (Attributes)
	{
		//call the attribute component's receive damage function
		Attributes->ReceiveDamage(DamageAmount);

		if (Attributes->IsNotAlive())
		{
			Die();
		}
	}
	return 0.0f;
}

void APlayerCharacter::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	Inits();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlap);
	SpinAttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlap);
	SpinAttackSwordHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlap);

	//spin attack movement params setup
	SpinAttackMovementParams.OnComplete.AddDynamic(this, &APlayerCharacter::SpinAttackMovementEnd);

	//set the character state to idle
	CharacterState = ECharacterState::ECS_Idle;

	TActorIterator<ASpawnPoint> SpawnPointIterator(GetWorld());
	ASpawnPoint* SpawnPoint = SpawnPointIterator ? *SpawnPointIterator : nullptr;

	if (SpawnPoint)
	{
		SetActorLocation(SpawnPoint->GetActorLocation());
	}

	TArray<AActor*> Objectives;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectivePoint::StaticClass(), Objectives);

	for(int i = 0; i < Objectives.Num(); i++)
	{
		if(!Objectives[i]->ActorHasTag("ObjectiveDisabled"))
		{
			ValidObjectives.Add(Objectives[i]);
		}
	}
	NumEnabledObjectivesTotal = ValidObjectives.Num();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	//call the parent implementation
	Super::SetupPlayerInputComponent(InInputComponent);

	//cast the player input component to an enhanced input component and check if the cast was successful
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InInputComponent))
	{
		//bind the input actions
		EnhancedInputComponent->BindAction(IA_GroundMovement, ETriggerEvent::Triggered, this, &APlayerCharacter::GroundMovement);
		EnhancedInputComponent->BindAction(IA_CameraMovement, ETriggerEvent::Triggered, this, &APlayerCharacter::CameraMovement);
		EnhancedInputComponent->BindAction(IA_DoJump, ETriggerEvent::Triggered, this, &APlayerCharacter::DoJump);
		EnhancedInputComponent->BindAction(IA_StopJump, ETriggerEvent::Triggered, this, &APlayerCharacter::StopJumpInput);
		EnhancedInputComponent->BindAction(IA_ShootGrapple, ETriggerEvent::Triggered, this, &APlayerCharacter::ShootGrapple);
		EnhancedInputComponent->BindAction(IA_StopGrapple, ETriggerEvent::Triggered, this, &APlayerCharacter::StopGrapple);
		EnhancedInputComponent->BindAction(IA_SpinAttack, ETriggerEvent::Triggered, this, &APlayerCharacter::DoSpinAttack);
		EnhancedInputComponent->BindAction(IA_RespawnButton, ETriggerEvent::Triggered, this, &APlayerCharacter::CallRestartPlayer);
		EnhancedInputComponent->BindAction(IA_RotationToggleOn, ETriggerEvent::Triggered, this, &APlayerCharacter::RotationToggleOn);
		EnhancedInputComponent->BindAction(IA_RotationToggleOff, ETriggerEvent::Triggered, this, &APlayerCharacter::RotationToggleOff);
		//EnhancedInputComponent->BindAction(IA_KillSelf, ETriggerEvent::Triggered, this, &APlayerCharacter::Destroyed);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	//call the parent implementation
	Super::Tick(DeltaTime);

	//EnergyRegeneration();
}

void APlayerCharacter::GroundMovement(const FInputActionValue& Value)
{
	//check if we can't use inputs
	if (CharacterState == ECharacterState::ECS_Dead)
	{
		return;
	}

	//get the vector direction from the input value
	const FVector2D VectorDirection = Value.Get<FVector2D>();

	//check if we're in the rotation modenumEnemiesDestroyed
	if (PlayerMovementComponent->bRotationMode)
	{
		//add w and s movement input
		AddMovementInput(FVector(PlayerMovementComponent->RotationSpeed, 0, 0), VectorDirection.Y);

		//add a and d movement input
		AddMovementInput(FVector(0, 0, PlayerMovementComponent->RotationSpeed), VectorDirection.X);

		return;
	}

	//get the control rotation and set the pitch and roll to zero
	const FRotator ControlPlayerRotationYaw = GetControlRotation();
	const FRotator YawPlayerRotation(0.f, ControlPlayerRotationYaw.Yaw, 0.f);

	//check if the player is grappling
	if (PlayerMovementComponent->bIsGrappling)
	{
		//get the up vector from the control rotation
		const FVector PlayerDirectionYaw_Upwards_Downwards = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::Z);

		//get the X axis for the movement input
		const FVector MovementXAxis = FVector::CrossProduct(PlayerDirectionYaw_Upwards_Downwards.GetSafeNormal(), PlayerMovementComponent->GrappleDirection.GetSafeNormal()).GetSafeNormal();

		//get the right vector from the control rotation
		const FVector PlayerDirectionYaw_Left_Right = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::Y);

		//get the X axis for the movement input
		const FVector MovementYAxis = FVector::CrossProduct((PlayerDirectionYaw_Left_Right * -1).GetSafeNormal(), PlayerMovementComponent->GrappleDirection.GetSafeNormal()).GetSafeNormal();

		//add upwards/downwards movement input
		AddMovementInput(MovementYAxis, VectorDirection.Y);

		//add left/right movement input
		AddMovementInput(MovementXAxis, VectorDirection.X);

		return;
	}

	//get the forward vector from the control rotation
	const FVector PlayerDirectionYaw_Forward_Backward = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::X);

	//get the right vector from the control rotation
	const FVector PlayerDirectionYaw_Left_Right = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::Y);

	//add forward/backwards movement input
	AddMovementInput(PlayerDirectionYaw_Forward_Backward, VectorDirection.Y);

	//add left/right movement input
	AddMovementInput(PlayerDirectionYaw_Left_Right, VectorDirection.X);
}

void APlayerCharacter::CameraMovement(const FInputActionValue& Value)
{
	//check if we can use the input
	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
	{
		//get the look axis input
		const FVector2D LookAxisInput = Value.Get<FVector2D>();

		//add controller yaw and pitch input
		AddControllerYawInput(LookAxisInput.X);
		AddControllerPitchInput(-LookAxisInput.Y);
	}
}

void APlayerCharacter::DoJump(const FInputActionValue& Value)
{
	//check if we can use the input
	if (CharacterState == ECharacterState::ECS_Dead)
	{
		return;
	}

	//check if we can't jump using our variable
	if (!bCanJump)
	{
		return;
	}

	//call the jump function
	Jump();
}

void APlayerCharacter::StopJumpInput(const FInputActionValue& Value)
{
	//check if we can use the input
	if (CharacterState == ECharacterState::ECS_Dead)
	{
		return;
	}

	//call the stop jump function
	StopJumping();
}

void APlayerCharacter::RotationToggleOn(const FInputActionValue& Value)
{
	//check if we can use the input
	if (CharacterState != ECharacterState::ECS_Dead)
	{
		//toggle the rotation mode
		PlayerMovementComponent->ToggleRotationMode(true);
	}
}

void APlayerCharacter::RotationToggleOff(const FInputActionValue& Value)
{
	//check if we can use the input
	if (CharacterState != ECharacterState::ECS_Dead)
	{
		//toggle the rotation mode
		PlayerMovementComponent->ToggleRotationMode(false);
	}
}

void APlayerCharacter::DoSpinAttack(const FInputActionValue& Value)
{
	//check if we're not dead
	if (CharacterState != ECharacterState::ECS_Dead && bCanSpinAttack)
	{
		//set the spin attack flag to true
		bIsSpinAttacking = true;

		//set the cqan spin attack flag to false
		bCanSpinAttack = false;

		//prevent the player from activating rotation mode
		PlayerMovementComponent->bCanActivateRotationMode = false;

		//disable orient rotation to movement
		GetCharacterMovement()->bOrientRotationToMovement = false;

		//set timer to reset the spin attack flag
		GetWorldTimerManager().SetTimer(SpinAttackTimer, this, &APlayerCharacter::SpinAttackEnd, SpinAttackDuration, false);

		//set the player's rotation
		SetActorRotation(FRotator(0.f, GetControlRotation().Yaw, GetControlRotation().Roll));

		//iterate through the overlapping actors of the spin attack hitboxes and call the spin attack on them
		TArray<AActor*> OverlappingActors;
		SpinAttackSwordHitbox->GetOverlappingActors(OverlappingActors);
		for (AActor* Actor : OverlappingActors)
		{
			DoSpinAttackOnEnemy(Actor);
		}
		SpinAttackHitbox->GetOverlappingActors(OverlappingActors);
		for (AActor* Actor : OverlappingActors)
		{
			DoSpinAttackOnEnemy(Actor);
		}

		//call the on spin attack event
		OnSpinAttackBegin();
	}
}

void APlayerCharacter::SpinAttackEnd()
{
	//check if we're not spin attacking
	if (!bIsSpinAttacking)
	{
		//return
		return;
	}

	//set bIsSpinAttacking to false
	bIsSpinAttacking = false;

	//re enable the player's ability to activate rotation mode
	PlayerMovementComponent->bCanActivateRotationMode = true;

	//re enable orient rotation to movement
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//empty the overlapped actors array
	SpinAttackOverlappedActors.Empty();

	//set a timer to end the cooldown of the character's spin attack
	GetWorldTimerManager().SetTimer(SpinAttackCooldownTimer, this, &APlayerCharacter::SpinAttackCooldownEnd, SpinAttackCooldownTime, false);

	//set the direction of the spin attack movement
	SpinAttackMovementParams.WorldDirection = GetControlRotation().Vector();

	//get the spin attack movement
	UAsyncRootMovement::AsyncRootMovement(this, PlayerMovementComponent, SpinAttackMovementParams);

	//call the blueprint event
	OnSpinAttackEnd();
}

void APlayerCharacter::SpinAttackCooldownEnd()
{
	//set bCanSpinAttack to true
	bCanSpinAttack = true;

	//call the blueprint event
	OnSpinAttackCooldownEnd();
}

void APlayerCharacter::SpinAttackMovementEnd()
{
	//check if we've not hit an enemy with the spin attack
	if (!bHitEnemyWithSpinAttack)
	{
		//clamp our velocity by the spin attack punishment
		PlayerMovementComponent->Velocity = PlayerMovementComponent->Velocity.GetClampedToMaxSize(SpinAttackPunishmentSpeed);
	}

	//reset whether or not we've hit an enemy with the spin attack
	bHitEnemyWithSpinAttack = false;
}

void APlayerCharacter::DoSpinAttackOnEnemy(AActor* Enemy)
{
	//check that the actor is valid and isn't the player
	if (!Enemy || Enemy == this)
	{
		return;
	}

	//check if the enemy is not valid and dosn't have the enemy tag or object tag
	if (!(Enemy->ActorHasTag(FName("Enemy")) || Enemy->ActorHasTag(FName("Object"))))
	{
		//call the non enemy hit event
		OnSpinAttackHitNonEnemy(Enemy);

		return;
	}

	//set the bHitEnemyWithSpinAttack flag to true
	bHitEnemyWithSpinAttack = true;

	//call the blueprint event
	OnSpinAttackHit(Enemy);

	//apply damage to the enemy
	UGameplayStatics::ApplyDamage(Enemy, Damage, GetController(), this, UDamageType::StaticClass());

	//Increase kills by one
	numEnemiesDestroyed += 1;
}

//bool APlayerCharacter::CrosshairCheck() const
//{
//	//if the player is already grappling return false
//	if (PlayerMovementComponent->bIsGrappling)
//	{
//		return false;
//	}
//
//	//if the player can grapple and there is no grappling hook return true
//	if (PlayerMovementComponent->CanGrapple() && (!GrapplingHookRef || GrapplingHookRef->IsActorBeingDestroyed()))
//	{
//		return true;
//	}
//
//	return false;
//}

void APlayerCharacter::ShootGrapple(const FInputActionValue& Value)
{
	//check if the input is used and if the character is not dead
	if (CharacterState == ECharacterState::ECS_Dead)
	{
		return;
	}

	//check if there is an existing grappling hook
	if (GrapplingHookRef)
	{
		//destroy the grappling hook
		GrapplingHookRef->Destroy();
	}

	//spawn the grappling hook
	SpawnGrappleProjectile();
}

void APlayerCharacter::StopGrapple(const FInputActionValue& Value)
{
	//check if we're dead
	if(CharacterState == ECharacterState::ECS_Dead)
	{
		return;
	}

	//stop grappling
	PlayerMovementComponent->StopGrapple();

	//check if there is an existing grappling hook
	if (!GrapplingHookRef)
	{
		return;
	}

	//check if we should destroy the hook immediately
	if (DestroyHookImmediately)
	{
		//destroy the grappling hook
		GrapplingHookRef->Destroy();

		//prevent further execution
		return;
	}

	//set the grappling hook to destroy on impact if it isn't already
	GrapplingHookRef->bDestroyOnImpact = true;
}

//void APlayerCharacter::PlayerDashAttack(const FInputActionValue& Value)
//{
//	if (/*(GetWorld()->GetTimeSeconds() - LastActionTime) >= CooldownDuration ||*/ DashEnergy > 100)
//	{
//		//check if we can use the input
//		if (CharacterState != ECharacterState::ECS_Dead)
//		{
//
//			// Get the camera manager
//			const APlayerCameraManager* CamManager = GetNetOwningPlayer()->GetPlayerController(GetWorld())->
//				PlayerCameraManager;
//
//			// Get the camera forward vector
//			const FVector CamForwardVec = CamManager->GetCameraRotation().Vector();
//
//			// Play the dash sound
//			UGameplayStatics::PlaySound2D(this, DashSound);
//
//			// Play niagara effect at socket location
//			UNiagaraFunctionLibrary::SpawnSystemAttached(DashEffect, Camera, FName("Dash"), FVector(EffectXLocation, EffectYLocation, EffectZLocation), FRotator(EffectPitch, EffectYaw, EffectRoll), EAttachLocation::KeepRelativeOffset, true);
//
//			// Get velocity from PlayerMovementComponent
//			const FVector Velocity = PlayerMovementComponent->Velocity;
//
//			//launch the character
//			this->LaunchCharacter(CamForwardVec * 100.f * DashSpeed, true, true);
//
//			PlayerMovementComponent->Velocity *= Velocity.GetSafeNormal() * FMath::Clamp(Velocity.Length(), FMath::Min(Velocity.Size(), MaxDashSpeed), FMath::Max(Velocity.Size(), MaxDashSpeed));
//
//			// Update the last action time
//			LastActionTime = GetWorld()->GetTimeSeconds();
//
//			// Decrement the amount of dashes
//			DashEnergy -= 100;
//
//			// Set a timer to reset the cooldown flag after CooldownDuration seconds
//			FTimerHandle CooldownTimerHandle;
//			GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &APlayerCharacter::ResetCooldownDashOne,
//				DashDuration, false);
//			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, FString::Printf(TEXT("Dash One Used")));
//
//			//Enable is dashing
//			bIsDashing = true;
//		}
//	}
//}
//
//void APlayerCharacter::ResetCooldownDashOne()
//{
//	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, FString::Printf(TEXT("Dash One Ready")));
//	bIsDashing = false;
//}
//
//void APlayerCharacter::EnergyRegeneration()
//{
//	DashEnergy += 1 * EnergyRegenerationRate;
//	DashEnergy = FMath::Clamp(DashEnergy, 0, MaximumDashEnergy);
//}

void APlayerCharacter::Destroyed()
{
	//call the parent implementation
	Super::Destroyed();

	//check if the world is valid
	if (GetWorld())
	{
		//get the game mode and check if it is valid
		if (const TObjectPtr<ABladebotGameMode> GameMode = Cast<ABladebotGameMode>(GetWorld()->GetAuthGameMode()))
		{
			//broadcast the on player death event
			GameMode->GetOnPlayerDeath().Broadcast(this);
			//GetWorld()->GetTimerManager().SetTimer(RespawnTime, this, &APlayerCharacter::CallRestartPlayer, 3.f, false);
		}
	}
}

void APlayerCharacter::CallRestartPlayer()
{
	//unpause the game
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	//Getting the World and GameMode in the world to invoke the restart player function
	if (const TObjectPtr<UWorld> World = GetWorld())
	{
		if (const TObjectPtr<ABladebotGameMode> GameMode = Cast<ABladebotGameMode>(World->GetAuthGameMode()))
		{
			//Getting Pawn Controller reference
			const TObjectPtr<AController> ControllerReference = GetController();

			//calling the RestartPlayer function
			GameMode->RestartPlayer(ControllerReference);

			//reset the player location
			AActor* PlayerStart = GameMode->FindPlayerStart(ControllerReference, TEXT("PlayerStart"));

			//set the player location to the player start location
			SetActorLocation(PlayerStart->GetActorLocation());

			//set the player rotation to the player start rotation
			SetActorRotation(PlayerStart->GetActorRotation());

			//set the player character's velocity to zero
			PlayerMovementComponent->Velocity = FVector::ZeroVector;

			//set should fire on stop grapple to false
			this->bShouldFireOnGrappleStop = false;

			//call the stop grappling function
			StopGrapple(FInputActionValue());

			//call the blueprint event
			OnRespawn();
		}
	}
	//Destroy();
}

// Player Spesific Functions
void APlayerCharacter::Die()
{
	//call the parent implementation
	Super::Die();

	//if we can't die, don't do anything
	if (CanDie == false)
	{
		return;
	}

	//set the character state to dead
	CharacterState = ECharacterState::ECS_Dead;

	//call the blueprint event
	OnDeath();
}

void APlayerCharacter::StopJumping()
{
	//call the parent implementation
	Super::StopJumping();

	//call the blueprint event
	OnJumpStop();
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	//check if the player movement component says we can jump
	if (PlayerMovementComponent->CanJumpAnyway())
	{
		//return true
		return true;
	}

	//otherwise return the parent implementation
	return Super::CanJumpInternal_Implementation();
}

void APlayerCharacter::SetPlayerDefaults()
{
	//call the parent implementation
	Super::SetPlayerDefaults();

	//set game complete to false
	GameComplete = false;

	//set num enemies destroyed to 0
	numEnemiesDestroyed = 0;

	//set num objectives complete to 0
	NumCompletes = 0;

	//set should fire on stop grapple to true
	bShouldFireOnGrappleStop = true;

	TActorIterator<ASpawnPoint> SpawnPointIterator(GetWorld());
	ASpawnPoint* SpawnPoint = SpawnPointIterator ? *SpawnPointIterator : nullptr;

	if (SpawnPoint)
	{
		//reset the camera rotation (by setting the control rotation)
		GetController()->SetControlRotation(SpawnPoint->GetActorRotation());
	}
}

void APlayerCharacter::CheckIfObjectivesComplete(AObjectivePoint* Objective)
{
	NumCompletes++;

	if(Objective->RequiresOrder == true && Objective->OrderIndex == expextedOrder)
	{
		expextedOrder++;
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("In order"));
	}
	else if(Objective->RequiresOrder == true)
	{

		Tags.Remove(FName("ObjectiveComplete"));


		Objective->AlreadyHit = false;
		Objective->ObjectiveComplete = false;
		if(Objective->shouldBeDestroyed)
		{
			Objective->ObjectveMesh->SetVisibility(true);
			Objective->ObjectveMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Objective->AfterActivationMesh->SetVisibility(false);
			Objective->AfterActivationMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		NumCompletes--;
	}

	if (NumCompletes == ValidObjectives.Num())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Game Won"));
		ObjectiveComplete();
		GameComplete = true;
	}

}

void APlayerCharacter::SpawnGrappleProjectile()
{
	//check if we have a valid grappling hook head class
	if (GrappleHookHeadClass)
	{
		//get the forward vector of the camera
		const FVector CamForwardVec = Camera->GetForwardVector();

		//get the spawn rotation
		const FRotator SpawnRotation = CamForwardVec.Rotation();

		//get the spawn location
		const FVector SpawnLocation = GetActorLocation() + CamForwardVec * GrappleSpawnDist;

		//spawn parameters
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//assign the grappling hook head reference to the spawned grappling hook head
		GrapplingHookRef = GetWorld()->SpawnActor<AGrapplingHookHead>(GrappleHookHeadClass, SpawnLocation,
			SpawnRotation, SpawnParams);

		//stop the player grapple
		PlayerMovementComponent->StopGrapple();

		//call the blueprint event
		OnShootGrapple();
	}
}

// Inits

void APlayerCharacter::Inits()
{
	InputInit();

	//bind blueprint events
	PlayerMovementComponent->OnNormalJump.AddDynamic(this, &APlayerCharacter::OnNormalJumpStart);
	PlayerMovementComponent->OnDirectionalJump.AddDynamic(this, &APlayerCharacter::OnDirectionalJumpStart);
	PlayerMovementComponent->OnCorrectedDirectionalJump.AddDynamic(this, &APlayerCharacter::OnCorrectedDirectionalJump);
	PlayerMovementComponent->OnStartGrapple.AddDynamic(this, &APlayerCharacter::OnStartGrapple);
	PlayerMovementComponent->OnWallLatch.AddDynamic(this, &APlayerCharacter::OnWallLatch);
	PlayerMovementComponent->OnWallLatchLaunch.AddDynamic(this, &APlayerCharacter::OnWallLatchLaunch);
	PlayerMovementComponent->OnWallLatchFall.AddDynamic(this, &APlayerCharacter::OnWallLatchFall);
	PlayerMovementComponent->OnWallRunStart.AddDynamic(this, &APlayerCharacter::OnWallRunStart);
	PlayerMovementComponent->OnWallRunJump.AddDynamic(this, &APlayerCharacter::OnWallRunJump);
	PlayerMovementComponent->OnWallRunFinish.AddDynamic(this, &APlayerCharacter::OnWallRunFinish);
}

void APlayerCharacter::UpdateObjectiveEnemyVariables()
{
	//set the number of enemies destroyed to 0
	numEnemiesDestroyed = 0;

	//set the number of objectives complete to 0
	NumCompletes = 0;

	TArray<AActor*> Objectives;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectivePoint::StaticClass(), Objectives);

	for(int i = 0; i < Objectives.Num(); i++)
	{
		if(!Objectives[i]->ActorHasTag("ObjectiveDisabled"))
		{
			ValidObjectives.Add(Objectives[i]);
		}
	}
	NumEnabledObjectivesTotal = ValidObjectives.Num();
}

void APlayerCharacter::InputInit()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// Input system
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer());
		if (Subsystem) Subsystem->AddMappingContext(IMC, 0);
	}
}

void APlayerCharacter::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor->ActorHasTag(FName("Enemy")) || OtherActor->ActorHasTag(FName("Object"))) && bIsSpinAttacking)
	{
		//set the bHitEnemyWithSpinAttack flag to true
		bHitEnemyWithSpinAttack = true;

		//check if the other actor has already been overlapped by one of the spin attack hitboxes
		if (SpinAttackOverlappedActors.Contains(OtherActor))
		{
			//return
			return;
		}

		//add the other actor to the overlapped actors array
		SpinAttackOverlappedActors.Add(OtherActor);

		//call the blueprint event
		OnSpinAttackHit(OtherActor);

		//apply damage to the other actor
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetController(), this, UDamageType::StaticClass());
		if(OtherActor->ActorHasTag(FName("Enemy")))
		{
			//Increase kills by one
			numEnemiesDestroyed += 1;
		}
	}
}
