#include "Characters/PlayerCharacter.h"
#include "GrapplingHook/GrapplingHookHead.h"
#include "Components/AttributeComponent.h"
#include "HUD/MainHUD.h"
#include "HUD/PlayerOverlay.h"
#include "Engine/DamageEvents.h"

//Components
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

//Gameplay Statics
#include "EnhancedInputComponent.h"
#include "Controllers/BladebotPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include <EnhancedInputSubsystems.h>
#include "BladebotGameMode.h"
#include "Components/CameraArmComponent.h"
#include "Components/PlayerMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerMovementComponent>(CharacterMovementComponentName))
{
	//Enable ticking
	PrimaryActorTick.bCanEverTick = true;

	//Detach rotation from controller
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//detach rotation from movement and set rotation rate
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	//get a reference to the movement component as a player movement component
	PlayerMovementComponent = Cast<UPlayerMovementComponent>(GetCharacterMovement());

	//set relative location and rotation for the mesh
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -60.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	//set the capsule component's default size
	GetCapsuleComponent()->SetCapsuleHalfHeight(60);
	GetCapsuleComponent()->SetCapsuleRadius(10);

	//create the camera arm
	CameraArm = CreateDefaultSubobject<UCameraArmComponent>(TEXT("CameraArm"));

	//attach it to the root component
	CameraArm->SetupAttachment(GetRootComponent());

	//set the camera arm's relative location to be above the character and a little behind
	CameraArm->SetRelativeLocation(FVector(0.f, 10.f, 90.f));

	////set the camera arm's relative rotation to be facing the character
	//CameraArm->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	//set the target arm length
	CameraArm->TargetArmLength = 400.f;

	//enable camera lag
	CameraArm->bEnableCameraLag = true;

	//set the camera lag speed
	CameraArm->CameraLagSpeed = 20.f;

	//make the camera follow the controller's rotation (so it uses the rotation input from the mouse)
	CameraArm->bUsePawnControlRotation = true;

	//create the camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	//attach it to the camera arm
	Camera->SetupAttachment(CameraArm);

	//detach the camera's rotation from the controller's rotation
	Camera->bUsePawnControlRotation = false;

	//create the attribute component
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	////spawn the grappling hook head
	//GrapplingHookRef = CreateDefaultSubobject<AGrapplingHookHead>(TEXT("GrapplingHookRef"));

	//default to automatically possessing the player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//add the player tag
	Tags.Add(FName("Player"));
}

float APlayerCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//check if we have a valid attribute component and player overlay
	if (Attributes && PlayerOverlay)
	{
		//call the attribute component's receive damage function
		Attributes->ReceiveDamage(DamageAmount);

		//update the health bar
		PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());

		//check if the player is dead
		if (Attributes->IsNotAlive())
		{
			//die
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

	//set the character state to idle
	CharacterState = ECharacterState::ECS_Idle;

	DebugPrint(this, 15, 1, FColor::Purple, FString::Printf(TEXT("Current movementspeed : %f"), GetVelocity().Size()));
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
		EnhancedInputComponent->BindAction(IA_ShootGrapple, ETriggerEvent::Triggered, this, &APlayerCharacter::ShootGrapple);
		EnhancedInputComponent->BindAction(IA_DespawnGrapple, ETriggerEvent::Triggered, this, &APlayerCharacter::DespawnGrapple);
		EnhancedInputComponent->BindAction(IA_DashAttack, ETriggerEvent::Triggered, this, &APlayerCharacter::PlayerDashAttack);
		EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
		EnhancedInputComponent->BindAction(IA_RespawnButton, ETriggerEvent::Triggered, this, &APlayerCharacter::CallRestartPlayer);
	}
}

bool APlayerCharacter::CanUseInput(const FInputActionValue& Value)
{
	//check if the input is used and if the character is not dead
	return Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead;
}

void APlayerCharacter::InputDebugMessage(const UInputAction* InputAction, const FString& DebugMessage)
{
	//check if debug mode is on
	DefaultDebugPrint(this, InputAction->GetName().Append(" " + DebugMessage));
}

void APlayerCharacter::GroundMovement(const FInputActionValue& Value)
{
	//check if we can use the input
	if (CanUseInput(Value))
	{
		//get the vector direction from the input value
		const FVector2D VectorDirection = Value.Get<FVector2D>();

		//get the control rotation and set the pitch and roll to zero
		const FRotator ControlPlayerRotationYaw = GetControlRotation();
		const FRotator YawPlayerRotation(0.f, ControlPlayerRotationYaw.Yaw, 0.f);

		//get the forward and right vectors from the control rotation
		const FVector PlayerDirectionYaw_Forward_Backward = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::X);
		const FVector PlayerDirectionYaw_Left_Right = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::Y);

		//add movement input
		AddMovementInput(PlayerDirectionYaw_Forward_Backward, VectorDirection.Y);
		AddMovementInput(PlayerDirectionYaw_Left_Right, VectorDirection.X);
	}
}

void APlayerCharacter::CameraMovement(const FInputActionValue& Value)
{
	//check if we can use the input
	if (CanUseInput(Value))
	{
		//print debug message
		InputDebugMessage(IA_CameraMovement);

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
	if (CanUseInput(Value))
	{
		//call the jump function
		Jump();

		//print debug message
		InputDebugMessage(IA_DoJump);
	}
}

void APlayerCharacter::ShootGrapple(const FInputActionValue& Value)
{
	//check if the input is used and if the character is not dead
	if (CanUseInput(Value))
	{
		//check if there is an existing grappling hook
		if (GrapplingHookRef)
		{
			//reactivate the grappling hook
			GrapplingHookRef->Reactivate(Camera->GetForwardVector() * GrappleSpeed);

			//print debug message
			InputDebugMessage(IA_ShootGrapple, TEXT("Reactivated"));
		}
		//there is no existing grappling hook
		else
		{
			//spawn the grappling hook
			SpawnGrappleProjectile();

			//immediately reactivate the grappling hook
			GrapplingHookRef->Reactivate(Camera->GetForwardVector() * GrappleSpeed);

			//print debug message
			InputDebugMessage(IA_ShootGrapple, TEXT("Spawned"));
		}
	}
}

void APlayerCharacter::DespawnGrapple(const FInputActionValue& Value)
{
	//check if we can use the input
	if (CanUseInput(Value))
	{
		//check if there is an existing grappling hook
		if (GrapplingHookRef)
		{
			//despawn the grappling hook
			GrapplingHookRef->Despawn();

			//print debug message
			InputDebugMessage(IA_DespawnGrapple);
		}
	}
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	//check if we can use the input
	if (CanUseInput(Value))
	{
		//print debug message
		InputDebugMessage(IA_Attack);

		// Apply damage function through gameplaystatics funciton
		//UGameplayStatics::ApplyDamage(
		//Actor that did damge
		//Amount of damage as float
		// SetOwner(this) is the owner of this attack
		// SetInstigator(this) is the instigator
		// UDamageType::StaticClass()
		//);
	}
}

void APlayerCharacter::PlayerDashAttack(const FInputActionValue& Value)
{
	//check if we can use the input
	if (CanUseInput(Value))
	{
		//print debug message
		InputDebugMessage(IA_DashAttack);

		//get the camera manager
		const APlayerCameraManager* CamManager = GetNetOwningPlayer()->GetPlayerController(GetWorld())->PlayerCameraManager;

		//get the camera forward vector
		const FVector CamForwardVec = CamManager->GetCameraRotation().Vector();

		//play the dash sound
		UGameplayStatics::PlaySound2D(this, DashSound);

		//launch the character
		//this->LaunchCharacter(CamForwardVec * 10.f * DashSpeed, true, true);
		//this->PlayerMovementComponent->bIsDashing = true;
		PlayerMovementComponent->Velocity += CamForwardVec * DashSpeed;
		GetWorldTimerManager().ClearTimer(PlayerMovementComponent->DashTimeHandler);
		GetWorldTimerManager().SetTimer(PlayerMovementComponent->DashTimeHandler, this, &APlayerCharacter::DashCheck, PlayerMovementComponent->DashTime, false);
	}
}

void APlayerCharacter::DashCheck()
{
	//bIsDashing = false;
			//get the camera manager
	const APlayerCameraManager* CamManager = GetNetOwningPlayer()->GetPlayerController(GetWorld())->PlayerCameraManager;

	//get the camera forward vector
	const FVector CamForwardVec = CamManager->GetCameraRotation().Vector();

	PlayerMovementComponent->Velocity -= CamForwardVec * DashSpeed;
}

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
		}
	}
}

void APlayerCharacter::CallRestartPlayer()
{
	//Getting Pawn Controller reference
	const TObjectPtr<AController> ControllerReference = GetController();

	//Destroying Player
	Destroyed();

	//Getting the World and GameMode in the world to invoke the restart player function
	if (const TObjectPtr<UWorld> World = GetWorld())
	{
		if (const TObjectPtr<ABladebotGameMode> GameMode = Cast<ABladebotGameMode>(World->GetAuthGameMode()))
		{
			GameMode->RestartPlayer(ControllerReference);
		}
	}
	Destroy();
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

	//print debug message
	DebugPrint(this, -1, 1, FColor::Red, TEXT("Dead"));
}

void APlayerCharacter::CountTime()
{
	//should use timespan instead of int probably

	//if the timer shouldn't tick, don't do anything
	if (TimerShouldTick == false)
	{
		return;
	}

	//increment the seconds
	DisplaySeconds++;

	//convert seconds to minutes
	if (DisplaySeconds > 60)
	{
		DisplayMinutes++;
		DisplaySeconds = 0;
	}

	//set the seconds and minutes on the player overlay
	PlayerOverlay->SetSeconds(DisplaySeconds);
	PlayerOverlay->SetMinutes(DisplayMinutes);
}

void APlayerCharacter::SpawnGrappleProjectile()
{
	//check if we have a valid grappling hook head class
	if (GrappleHookHeadClass)
	{
		//spawn parameters for the grappling hook head
		FActorSpawnParameters SpawnParameters;

		//set the owner of the grappling hook head to this actor
		SpawnParameters.Owner = this;

		//set the Instigator of the grappling hook head to this actor
		SpawnParameters.Instigator = this;

		//get the spawn rotation
		const FRotator SpawnRotation = GetActorRotation();

		//get the camera forward vector
		FVector CameraForwardVector = Camera->GetForwardVector();

		//get the spawn location
		const FVector SpawnLocation = GetActorLocation() * CameraForwardVector * GrappleSpawnDist;

		//assign the grappling hook head reference to the spawned grappling hook head
		GrapplingHookRef = GetWorld()->SpawnActorDeferred<AGrapplingHookHead>(
			GrappleHookHeadClass,
			FTransform(SpawnRotation, SpawnLocation, FVector(1, 1, 1)),
			this,
			this,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		//set the spawn distance to use in reactivation later
		GrapplingHookRef->SpawnDistance = GrappleSpawnDist;

		//set the velocity to the camera forward vector
		GrapplingHookRef->InitialVelocity = Camera->GetForwardVector() * GrappleSpeed;

		//set the player movement component
		GrapplingHookRef->PlayerMovementComponent = PlayerMovementComponent;

		//finish spawning the grappling hook head
		GrapplingHookRef->FinishSpawning(FTransform(SpawnRotation, SpawnLocation, FVector(1, 1, 1)));
	}
}

// Inits

void APlayerCharacter::Inits()
{
	OverlayInit();
	TimerInit();
	InputInit();
	CharacterState = ECharacterState::ECS_Idle;
	Tags.Add(FName("Player"));
}

void APlayerCharacter::InputInit()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// Input system
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem) Subsystem->AddMappingContext(IMC, 0);
	}
}

void APlayerCharacter::OverlayInit()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		AMainHUD* MainHUD = Cast<AMainHUD>(PlayerController->GetHUD());

		if (MainHUD)
		{
			PlayerOverlay = MainHUD->GetMainOverlay();

			if (PlayerOverlay && Attributes)
			{
				PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				PlayerOverlay->SetSeconds(DisplaySeconds);
				PlayerOverlay->SetMinutes(DisplayMinutes);
				PlayerOverlay->EnableGrapplingCrosshair(false);
			}
		}
	}
}

void APlayerCharacter::TimerInit()
{
	GetWorldTimerManager().SetTimer(Seconds, this, &APlayerCharacter::CountTime, 1.f, true);
}

void APlayerCharacter::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(FName("Enemy")) && GetVelocity().Size() > MovementSpeedToKill && OtherComponent->GetCollisionObjectType() != ECC_WorldDynamic)
	{
		//GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("Killed em")));
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetController(), this, UDamageType::StaticClass());
	}
}