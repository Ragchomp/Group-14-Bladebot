
//Main
#include "Characters/PlayerCharacter.h"
#include "GrapplingHook/GrapplingHookHead.h"
#include "Components/AttributeComponent.h"
#include "HUD/MainHUD.h"
#include "HUD/PlayerOverlay.h"

//Components
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "CableComponent.h"

//Game play Statics
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubSystems.h"
#include "InputActionValue.h"

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

	// Attribute Component
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	//CableInit
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("CableComponent"));
	CableComponent->SetupAttachment(GetRootComponent());

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

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	if (Attributes && PlayerOverlay) {
		
		Attributes->ReciveDamage(DamageAmount);
		PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}

	return DamageAmount;
}

void APlayerCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);
	
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Yellow, TEXT("GetHit implementation"));
	if (Attributes->IsAlive()) {
		Die();
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	Inits();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DespawnGrappleIfAtTeatherMax();
	GrapplePhysicsUpdate();
	DetectIfCanGrapple();

	TimeManager();
	CableManager();

	//Makes sure grapplePhysics don't apply when not reeling
	TryingTooReel = false;

	//Debug tool for Distance
	if(DebugMode == true && GrapplingHookRef)
	{
		float Distance = GetDistanceBetweenTwoPoints(GetActorLocation(), GrapplingHookRef->GetActorLocation());
		GEngine->AddOnScreenDebugMessage(2, 1, FColor::Green, FString::Printf(TEXT("Distance from Grapple : %f"),Distance));
	}
}

// Input Functions

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
		// delete theis grapple pull update when called in grappeling hook head
		GrapplePullUpdate();
	}
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
	{
		if (DebugMode == true)
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Yellow, TEXT("Attack"));

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

// Player Spesific Functions

void APlayerCharacter::Die()
{
	Super::Die();

	if (CanDie == false) return;
	CharacterState = ECharacterState::ECS_Dead;

	if (DebugMode == true)
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Dead"));
}

// Tick Managers

void APlayerCharacter::TimeManager()
{
	if (TimerShouldTick == false) return;

	if (DisplaySeconds > 60)
	{
		DisplayMinutes++;
		DisplaySeconds = 0;
	}

	PlayerOverlay->SetSeconds(DisplaySeconds);
	PlayerOverlay->SetMinutes(DisplayMinutes);
}

void APlayerCharacter::CountSeconds()
{
	DisplaySeconds++;
}

void APlayerCharacter::CableManager()
{
	// Set Collision enabled for cable in bp, as code is still experimental
	if (GrapplingHookRef)
	{
		CableComponent->SetVisibility(true);
		// for moving the cable end position with the grappling hook
		CableComponent->SetAttachEndToComponent(GrapplingHookRef->GetRootComponent(), NAME_None);
		// for moving the cable start position down to the main body
		FTransform InitialTransform;
		InitialTransform.SetLocation(GetActorLocation() + FVector(0.f, 0.f, -40.f));
		CableComponent->SetWorldTransform(InitialTransform);
	}
	else
	{
		CableComponent->SetVisibility(false);
	}
}

// Grappling Logic

void APlayerCharacter::GetGrapplingHookRef()
{
	// Creating a class reference to all Grappling hooks in case of duplicates,
    // Uses slot 0 or first shot. Should always only be one
	TArray<AActor*> GrapplingHooks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_GrapplingHookHead, GrapplingHooks);
	GrapplingHookRef = Cast<AGrapplingHookHead>(GrapplingHooks[0]);
	if (GrapplingHookRef && DebugMode == true)
	{
		GEngine->AddOnScreenDebugMessage(0, 10, FColor::Yellow, GrapplingHookRef->GetName());
	}
}

void APlayerCharacter::SpawnGrappleProjectile()
{
	const FRotator SpawnRotation = FRotator(GetControlRotation().Pitch - 90, GetControlRotation().Yaw, GetControlRotation().Roll);
	const FVector SpownLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 10);

	if (BP_GrapplingHookHead)
		GetWorld()->SpawnActor<AGrapplingHookHead>(BP_GrapplingHookHead, SpownLocation, SpawnRotation);
}

void APlayerCharacter::GrapplePhysicsUpdate()
{
	if (IsMaxTeather == false) return;

	if (GrapplingHookRef && GrapplingHookRef->GetGrappleState() == EGrappleState::EGS_Attached)
	{
		const float Distance = GetDistanceBetweenTwoPoints(GrapplingHookRef->GetActorLocation(), GetActorLocation());

		FVector AnchorPoint = GrapplingHookRef->GetActorLocation();

		// Calculate the vector between the character's current location and the anchor point
		FVector CharacterToAnchor = GetActorLocation() - AnchorPoint;

		// Clamp the distance vector to the maximum allowed distance
		FVector ClampedVector = CharacterToAnchor.GetClampedToMaxSize(GrappleMaxDistance);

		// Set the character's new location after applying the constraint
		SetActorLocation(AnchorPoint + ClampedVector);

		if (Distance >= GrappleMaxDistance && TryingTooReel == true)
		{
			SetActorLocation(AnchorPoint + ClampedVector);
			GetCharacterMovement()->Velocity = FVector::ZeroVector;
		}
	}
}

void APlayerCharacter::GrapplePullUpdate()
{
	if (GrapplingHookRef->GetGrappleState() == EGrappleState::EGS_Attached)
	{
		FVector PullVector = GetVectorBetweenTwoPoints
			(GetActorLocation(),
		GrapplingHookRef->GetActorLocation());

		GetCharacterMovement()->AddImpulse(PullVector * PullStrenght);
	}
}

void APlayerCharacter::DetectIfCanGrapple()
{
	if (PlayerOverlay == nullptr) return;

	FHitResult OutHit;
	LineTrace(OutHit);

	if (OutHit.bBlockingHit)
	{
		PlayerOverlay->EnableGrapplingCrosshair(true);
		InGrappleRange = true;
	}
	else
	{
		PlayerOverlay->EnableGrapplingCrosshair(false);
		InGrappleRange = false;
	}

}

void APlayerCharacter::DespawnGrappleIfAtTeatherMax()
{
	if (IsMaxTeather == false) return;

	if (GrapplingHookRef)
	{
		if (GetDistanceBetweenTwoPoints(GrapplingHookRef->GetActorLocation(), GetActorLocation()) > GrappleMaxDistance
			&& GrapplingHookRef->GetGrappleState() != EGrappleState::EGS_Attached)
		{
			GrapplingHookRef->Despawn();
			IsRetracted = true;
		}
	}
}

// Tracers
void APlayerCharacter::LineTrace(FHitResult& OutHit)
{
	// Trace Information
	const FVector Start = GetActorLocation();
	const FVector End = GetPointWithRotator(Start, GetControlRotation(), GrappleMaxDistance);
	bool bTraceComplex = false;
	FHitResult LineHit;
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
		EDrawDebugTrace::None,
		OutHit,
		true
	);
	
	// This will hit anything hitable with the line trace
	// if the line trace hits something the first if is passed
	if (LineHit.GetActor())
	{
		// if this cast works it means what we hit has a hitinterface as well
		IHitInterface* HitInterface = Cast<IHitInterface>(LineHit.GetActor());
		if (HitInterface) {
			// it then calls what we hits; get hit function
			HitInterface->Execute_GetHit(LineHit.GetActor(), LineHit.ImpactPoint);
			// As long as a class that is supposed to get hit has hit interface 
			// inherited it will take damage from the attack
		}
	}
}

// Inits

void APlayerCharacter::Inits()
{
	InputInit();
	OverlayInit();
	TimerInit();
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
	GetWorldTimerManager().SetTimer(Seconds, this, &APlayerCharacter::CountSeconds, 1.f, true);
}

