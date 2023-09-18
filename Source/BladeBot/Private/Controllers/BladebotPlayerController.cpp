//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "Controllers/BladebotPlayerController.h"
//
////Input
//#include "EnhancedInputComponent.h"
//#include "EnhancedInputSubSystems.h"
//#include "InputActionValue.h"
//#include "GameFramework/Controller.h"
//#include <Kismet/GameplayStatics.h>
//#include "Characters/PlayerCharacter.h"
//#include "GrapplingHook/GrapplingHookHead.h"
//
//
//ABladebotPlayerController::ABladebotPlayerController()
//{
//
//}
//
//void ABladebotPlayerController::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	GrappleReel();
//}
//
//// Input Functions
//
//void ABladebotPlayerController::BeginPlay()
//{
//	Super::BeginPlay();
//
//	if (const APlayerController *Controller = Cast<APlayerController>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetController()))
//    {
//        if (UEnhancedInputLocalPlayerSubsystem *InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer()))
//        {
//            InputSystem->AddMappingContext(IMC, 0);
//        }
//    }
//
//	SetupPlayerInputComponent();
//
//	// Input system
//	//if (Subsystem) Subsystem->AddMappingContext(IMC, 0);
//
//}
//
//void ABladebotPlayerController::GroundMovement(const FInputActionValue& Value)
//{
//	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
//	{
//		FVector2D VectorDirection = Value.Get<FVector2D>();
//		
//		const FRotator ControlPlayerRotationYaw = GetControlRotation();
//		const FRotator YawPlayerRotation(0.f, ControlPlayerRotationYaw.Yaw, 0.f);
//
//		const FVector PlayerDirectionYaw_Forward_Backward = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::X);
//		const FVector PlayerDirectionYaw_Left_Right = FRotationMatrix(YawPlayerRotation).GetUnitAxis(EAxis::Y);
//
//		PlayerCharacter->AddMovementInput(PlayerDirectionYaw_Forward_Backward, VectorDirection.Y);
//		PlayerCharacter->AddMovementInput(PlayerDirectionYaw_Left_Right, VectorDirection.X);
//	}
//}
//
//void ABladebotPlayerController::CameraMovement(const FInputActionValue& Value)
//{
//	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
//	{
//		FVector2D LookAxisInput = Value.Get<FVector2D>();
//		PlayerCharacter->AddControllerYawInput(LookAxisInput.X);
//		PlayerCharacter->AddControllerPitchInput(-LookAxisInput.Y);
//	}
//}
//
//void ABladebotPlayerController::DoJump(const FInputActionValue& Value)
//{
//	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
//	{
//		PlayerCharacter->Jump();
//	}
//}
//
//void ABladebotPlayerController::ShootGrapple(const FInputActionValue& Value)
//{
//	
//	if(Value.IsNonZero() && PlayerCharacter->IsRetracted == true && CharacterState != ECharacterState::ECS_Dead)
//	{
//		PlayerCharacter->SpawnGrappleProjectile();
//
//		PlayerCharacter->GetGrapplingHookRef();
//
//		PlayerCharacter->IsRetracted = false;
//	}
//	else if(Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
//	{
//		if (GrapplingHookRef)
//		{
//			GrapplingHookRef->Despawn();
//			GEngine->AddOnScreenDebugMessage(0, 0.5, FColor::Red, GrapplingHookRef->GetName() + TEXT(" Deleted"));
//			GrapplingHookRef = nullptr;
//		}
//
//		PlayerCharacter->IsRetracted = true;
//	}
//}
//
//void ABladebotPlayerController::GrappleReel()
//{
//	if (GrapplingHookRef && GrapplingHookRef->GetGrappleState() != EGrappleState::EGS_Retracted
//		&& CharacterState != ECharacterState::ECS_Dead)
//	{
//		PlayerCharacter->TryingTooReel = true;
//		// delete this grapple pull update when called in grappeling hook head
//		PlayerCharacter->GrapplePullUpdate();
//	}
//}
//
//void ABladebotPlayerController::Attack(const FInputActionValue& Value)
//{
//	if (Value.IsNonZero() && CharacterState != ECharacterState::ECS_Dead)
//	{
//		/*if (DebugMode == true)
//		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Yellow, TEXT("Attack"));*/
//
//		// Apply damage function through gameplaystatics funciton
//		//UGameplayStatics::ApplyDamage(
//		//Actor that did damge
//		//Amount of damage as float
//		// SetOwner(this) is the owner of this attack
//		// SetInstigator(this) is the instigator
//		// UDamageType::StaticClass()
//		//);
//
//	}
//}