//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Characters/StateControl.h"
//#include "GameFramework/PlayerController.h"
//#include "BladebotPlayerController.generated.h"
//
//class UEnhancedInputLocalPlayerSubsystem;
//class APlayerCharacter;
//struct FInputActionValue;
//class UInputMappingContext;
//class UInputAction;
//
///**
// * 
// */
//UCLASS()
//class BLADEBOT_API ABladebotPlayerController : public APlayerController
//{
//	GENERATED_BODY()
//
//public:
//	ABladebotPlayerController();
//	virtual void Tick(float DeltaTime) override;
//
//	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
//	APlayerCharacter* PlayerCharacter;
//	//UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
//
//	/** Input Functions */
//	UFUNCTION()
//	void GroundMovement(const FInputActionValue& Value);
//
//	UFUNCTION()
//	void CameraMovement(const FInputActionValue& Value);
//
//	UFUNCTION()
//	void DoJump(const FInputActionValue& Value);
//
//	UFUNCTION()
//	void ShootGrapple(const FInputActionValue& Value);
//
//	UFUNCTION()
//	void GrappleReel();
//
//	UFUNCTION()
//	void Attack(const FInputActionValue& Value);
//
//		/** Input Calls */
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
//	class UInputMappingContext* IMC;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
//	class UInputAction* IA_GroundMovement;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
//	class UInputAction* IA_CameraMovement;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
//	class UInputAction* IA_DoJump;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
//	class UInputAction* IA_ShootGrapple;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
//	class UInputAction* IA_GrappleReel;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1-Inputsystem")
//	class UInputAction* IA_Attack;
//
//
//
//protected:
//	virtual void BeginPlay() override;
//
//	UPROPERTY()
//	class AGrapplingHookHead* GrapplingHookRef{ nullptr };
//
//
//		/** State Control  */
//	ECharacterState CharacterState = ECharacterState::ECS_Idle;
//
//public:	
//
//	/** Getters & Setters  */
//	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
//};
