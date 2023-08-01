
//Main
#include "Characters/PlayerCharacter.h"
#include "GrapplingHook/GrapplingHookHead.h"
#include "HUD/MainHUD.h"
#include "HUD/PlayerOverlay.h"

//Components
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

//Game play Statics
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubSystems.h"
#include "InputActionValue.h"

//Math Includes
#include "Math/Vector.h"
#include "Math/Rotator.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Orientation Init
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// Mesh Init
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -60.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	// Capsule Init
	GetCapsuleComponent()->SetCapsuleHalfHeight(60);
	GetCapsuleComponent()->SetCapsuleRadius(10);

	// Spring-Arm Init
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->SetRelativeLocation(FVector(0.f, 10.f, 90.f));
	SpringArm->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 20.f;

	// Camera Init
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// GrapplingHook Init
	GrapplingHookRef = CreateDefaultSubobject<AGrapplingHookHead>(TEXT("GrapplingHookRef"));

	// Player Possession
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_GroundMovement, ETriggerEvent::Triggered, this, &APlayerCharacter::GroundMovement);
		EnhancedInputComponent->BindAction(IA_CameraMovement, ETriggerEvent::Triggered, this, &APlayerCharacter::CameraMovement);
		EnhancedInputComponent->BindAction(IA_DoJump, ETriggerEvent::Triggered, this, &APlayerCharacter::DoJump);
		EnhancedInputComponent->BindAction(IA_ShootGrapple, ETriggerEvent::Triggered, this, &APlayerCharacter::ShootGrapple);
		EnhancedInputComponent->BindAction(IA_GrappleReel, ETriggerEvent::Triggered, this, &APlayerCharacter::GrappleReel);
		EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	CharacterState = ECharacterState::ECS_Idle;
	Tags.Add(FName("Player"));

	InputInit();

	InitOverlay();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DespawnGrappleIfAtTeatherMax();
	GrapplePhysicsUpdate();

	// Fix this?
	TryingTooReel = false;

	// Debug Distance
	if(GrapplingHookRef)
	{
		const float DistanceBetweenGrapAndPlayer = GetDistanceBetweenTwoPoints(GrapplingHookRef->GetActorLocation(), GetActorLocation());

		GEngine->AddOnScreenDebugMessage(0, 0.1f, FColor::Yellow, FString::Printf(TEXT("Distance is: %f"), DistanceBetweenGrapAndPlayer));
	}
}

void APlayerCharacter::GroundMovement(const FInputActionValue& Value)
{
	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
	{
		FVector2D VectorDirection = Value.Get<FVector2D>();
		
		const FRotator ControlPlayerRotationYaw = GetControlRotation();
		const FRotator YawPlayerRotation(0.f, ControlPlayerRotationYaw.Yaw, 0.f);

		const FVector PlayerDirectionYaw_Forward_Backward = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::X);
		const FVector PlayerDirectionYaw_Left_Right = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(PlayerDirectionYaw_Forward_Backward, VectorDirection.Y);
		AddMovementInput(PlayerDirectionYaw_Left_Right, VectorDirection.X);
	}
}

void APlayerCharacter::CameraMovement(const FInputActionValue& Value)
{
	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
	{
		FVector2D LookAxisInput = Value.Get<FVector2D>();
		AddControllerYawInput(LookAxisInput.X);
		AddControllerPitchInput(-LookAxisInput.Y);
	}
}

void APlayerCharacter::DoJump(const FInputActionValue& Value)
{
	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
	{
		Jump();
	}
}

void APlayerCharacter::ShootGrapple(const FInputActionValue& Value)
{
	
	if(Value.IsNonZero() && IsRetracted == true && CharacterState != ECharacterState::ECS_Dead)
	{
		FHitResult OutHit;
		LineTrace(OutHit);

		SpawnGrappleProjectile();

		GetGrapplingHookRef();

		IsRetracted = false;
	}
	else if(Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
	{
		if (GrapplingHookRef)
		{
			GrapplingHookRef->Despawn();
			GEngine->AddOnScreenDebugMessage(0, 0.5, FColor::Red, GrapplingHookRef->GetName() + TEXT(" Deleted"));
			GrapplingHookRef = nullptr;
		}

		IsRetracted = true;
	}
}

void APlayerCharacter::GrappleReel(const FInputActionValue& Value)
{
	if (Value.IsNonZero() && GrapplingHookRef && GrapplingHookRef->GetGrappleState() != EGrappleState::EGS_Retracted
		&& CharacterState != ECharacterState::ECS_Dead)
	{
		TryingTooReel = true;
		GrapplePullUpdate();
	}
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
	{
		GEngine->AddOnScreenDebugMessage(1, 1, FColor::Yellow, TEXT("Attack"));
		TakeDamage(10.f);
	}
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

void APlayerCharacter::InitOverlay()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("InitOverlay"));

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("GotController"));

		AMainHUD* MainHUD = Cast<AMainHUD>(PlayerController->GetHUD());
		if (MainHUD)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("GotMainHud"));

			PlayerOverlay = MainHUD->GetMainOverlay();

			if (PlayerOverlay)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("GotPlayerOverlay"));

				//PlayerOverlay->SetHealthBarPercent(GetHealthPercent());
				PlayerOverlay->SetHealthBarPercent(0.2f);
				PlayerOverlay->SetMinutes(1);
				PlayerOverlay->SetSeconds(2);
				PlayerOverlay->EnableGrapplingCrosshair(false);
			}
		}
	}
}

void APlayerCharacter::LineTrace(FHitResult& OutHit)
{
	// Trace Information
	const FVector Start = GetActorLocation();
	const FVector End = GetPointWithRotator(Start, GetControlRotation(), GrappleDistance);
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// The Trace
	UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true
	);

}

void APlayerCharacter::SpawnGrappleProjectile()
{
	const FRotator SpawnRotation = FRotator(GetControlRotation().Pitch - 90, GetControlRotation().Yaw, GetControlRotation().Roll);
	const FVector SpownLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 10);

	if (BP_GrapplingHookHead)
		GetWorld()->SpawnActor<AGrapplingHookHead>(BP_GrapplingHookHead, SpownLocation, SpawnRotation);
}

void APlayerCharacter::GetGrapplingHookRef()
{
	 //Creating a class reference to all Grappling hooks in case of duplicates,
	// Uses slot 0 or first shot. Should always only be one
	TArray<AActor*> GrapplingHooks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_GrapplingHookHead, GrapplingHooks);
	GrapplingHookRef = Cast<AGrapplingHookHead>(GrapplingHooks[0]);
	if (GrapplingHookRef)
	{
		GEngine->AddOnScreenDebugMessage(0, 10, FColor::Yellow, GrapplingHookRef->GetName());
	}
}

FVector APlayerCharacter::GetPointWithRotator(const FVector& Start, const FRotator& Rotation, float Distance)
{
	// Convert the rotation to a quaternion
	FQuat Quaternion = Rotation.Quaternion();

	// Create a direction vector from the quaternion
	FVector Direction = Quaternion.GetForwardVector();

	// Calculate the coordinates of the point using the direction vector and distance
	FVector Point = Start + (Direction * Distance);

	return Point;
}

FVector APlayerCharacter::GetVectorOfRotation(const FRotator& Rotation)
{
	// Convert the rotation to a quaternion
	FQuat Quaternion = Rotation.Quaternion();

	// Create a direction vector from the quaternion
	FVector Direction = Quaternion.GetForwardVector();

	return Direction;
}

FVector APlayerCharacter::GetVectorBetweenTwoPoints(const FVector& Point1, const FVector& Point2)
{
	FVector VectorBetweenLocations = Point1 - Point2;
	VectorBetweenLocations.Normalize();
	

	return -VectorBetweenLocations;
}

float APlayerCharacter::GetDistanceBetweenTwoPoints(const FVector& Point1, const FVector& Point2)
{
	float DistanceToTarget = FVector::Dist(Point1, Point2);

	return DistanceToTarget;
}

void APlayerCharacter::GrapplePhysicsUpdate()
{
	if (GrapplingHookRef && GrapplingHookRef->GetGrappleState() == EGrappleState::EGS_Attached)
	{
		const float Distance = GetDistanceBetweenTwoPoints(GrapplingHookRef->GetActorLocation(), GetActorLocation());
	
		FVector AnchorPoint = GrapplingHookRef->GetActorLocation();

		// Calculate the vector between the character's current location and the anchor point
		FVector CharacterToAnchor = GetActorLocation() - AnchorPoint;

		// Clamp the distance vector to the maximum allowed distance
		FVector ClampedVector = CharacterToAnchor.GetClampedToMaxSize(GrappleDistance);
			
		// Set the character's new location after applying the constraint
		SetActorLocation(AnchorPoint + ClampedVector);

		if(Distance >= GrappleDistance && TryingTooReel == true)
		{
			SetActorLocation(AnchorPoint + ClampedVector);
			GetCharacterMovement()->Velocity = FVector::ZeroVector;
		}
	}
}

void APlayerCharacter::GrapplePullUpdate()
{
	if(GrapplingHookRef->GetGrappleState()==EGrappleState::EGS_Attached)
	{
		FVector PullVector = GetVectorBetweenTwoPoints
							 (GetActorLocation(),
							 GrapplingHookRef->GetActorLocation());

		GetCharacterMovement()->AddImpulse(PullVector * PullStrenght);
	}
}

void APlayerCharacter::DespawnGrappleIfAtTeatherMax()
{
	if (GrapplingHookRef)
	{
		if (GetDistanceBetweenTwoPoints(GrapplingHookRef->GetActorLocation(), GetActorLocation()) > GrappleDistance 
										&& GrapplingHookRef->GetGrappleState() != EGrappleState::EGS_Attached)
		{
			GrapplingHookRef->Despawn();
			IsRetracted = true;
		}
	}
}

void APlayerCharacter::TakeDamage(float DamageAmount)
{
	
	//if(HUDWidget)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("2Took Damage"));
	//	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	//	HUDWidget->SetHealthPercent(GetHealthPercent());
	//}
}

float APlayerCharacter::GetHealthPercent()
{
	return CurrentHealth / MaxHealth;
}

bool APlayerCharacter::IsAlive()
{
	return CurrentHealth > 0.f;
}

void APlayerCharacter::Die()
{
	CharacterState = ECharacterState::ECS_Dead;
}


