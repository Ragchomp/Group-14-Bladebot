
//Main
#include "Characters/PlayerCharacter.h"
#include "GrapplingHook/GrapplingHookHead.h"

//Components
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

//Gameplay Statics
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
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	SpringArm->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 20.f;

	// Camera Init
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

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

	Tags.Add(FName("Player"));
	InputInit();

	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::GroundMovement(const FInputActionValue& Value)
{
	if (Value.IsNonZero())
	{
		//GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, TEXT("Groundmove"));
		FVector2D VectorDirection = Value.Get<FVector2D>();

		//We want to move in the direction of the Yaw rotation(x-look-axis). Fixing rotation to Yaw.
		const FRotator ControlPlayerRotationYaw = GetControlRotation();
		const FRotator YawPlayerRotation(0.f, ControlPlayerRotationYaw.Yaw, 0.f);

		//Calculated the UnitAxis. We normalize the vector and find the normalized vector.
		const FVector PlayerDirectionYaw_Forward_Backward = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::X);
		const FVector PlayerDirectionYaw_Left_Right = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(PlayerDirectionYaw_Forward_Backward, VectorDirection.Y);
		AddMovementInput(PlayerDirectionYaw_Left_Right, VectorDirection.X);
	}
}

void APlayerCharacter::CameraMovement(const FInputActionValue& Value)
{
	if (Value.IsNonZero())
	{
		//GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, TEXT("Camera move"));
		FVector2D LookAxisInput = Value.Get<FVector2D>();
		AddControllerYawInput(LookAxisInput.X);
		AddControllerPitchInput(-LookAxisInput.Y);
	}
}

void APlayerCharacter::DoJump(const FInputActionValue& Value)
{
	if (Value.IsNonZero())
	{
		//GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, TEXT("Jump"));
		Jump();
	}
}

void APlayerCharacter::ShootGrapple(const FInputActionValue& Value)
{

	if(Value.IsNonZero() && GrappleOut == false)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.5, FColor::Blue, TEXT("Shot Grapple"));

		//Line trace
		FHitResult OutHit;
		LineTrace(OutHit);

		GrappleOut = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(2, 0.5, FColor::Red, TEXT("Retract Grapple"));
		GrappleOut = false;
	}
}

void APlayerCharacter::GrappleReel(const FInputActionValue& Value)
{

	if (Value.IsNonZero() && GrappleOut == true)
	{
		GEngine->AddOnScreenDebugMessage(0, 0.5, FColor::Green, TEXT("Retract Grapple"));

	}
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (Value.IsNonZero())
	{
		GEngine->AddOnScreenDebugMessage(1, 1, FColor::Yellow, TEXT("Attack"));
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

void APlayerCharacter::LineTrace(FHitResult& OutHit)
{

	// Trace Information
	const FVector Start = GetActorLocation();
	const FVector End = GetPointWithRotator(Start,GetControlRotation(),GrappleDistance);
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



