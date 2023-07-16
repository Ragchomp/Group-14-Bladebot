
//Main
#include "Characters/PlayerCharacter.h"

//Components
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

//Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubSystems.h"
#include "InputActionValue.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 40.f, 0.f);


	// Mesh Init
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -60.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	// Capsule Component init
	GetCapsuleComponent()->SetCapsuleHalfHeight(60);
	GetCapsuleComponent()->SetCapsuleRadius(10);

	// Spring Arm init
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	SpringArm->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 20.f;

	// Camera init
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Player possession
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
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Player control init
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// Input system
		GEngine->AddOnScreenDebugMessage(1, 10, FColor::Yellow, TEXT("MappincontextAdded"));
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem) Subsystem->AddMappingContext(IMC, 0);
	}

	Tags.Add(FName("Player"));
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::GroundMovement(const FInputActionValue& Value)
{
	if (Controller && Value.IsNonZero())
	{
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
	if (Controller && Value.IsNonZero())
	{
		FVector2D LookAxisInput = Value.Get<FVector2D>();
		AddControllerYawInput(LookAxisInput.X);
		AddControllerPitchInput(-LookAxisInput.Y);
	}
}

void APlayerCharacter::DoJump(const FInputActionValue& Value)
{
	if (Controller && Value.IsNonZero())
	{
		Jump();
	}
}



