#include "Components/CameraArmComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCameraArmComponent::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//check if the owner is a character
	if (GetOwner()->IsA(ACharacter::StaticClass()))
	{
		//cast the owner to a character
		CharacterOwner = Cast<ACharacter>(GetOwner());
	}
	else
	{
		//print an error message
		UE_LOG(LogTemp, Error, TEXT("The owner of the CameraArmComponent is not a character!"));
	}
}

void UCameraArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	//check if bDoLocationLag is true
	if (bDoLocationLag)
	{
		//get the owner of the component's velocity
		float OwnerSpeed = GetOwner()->GetVelocity().Length();

		//setup the lag min velocity
		float LagMinVelocity;

		//check if bUseCharacterWalkingSpeed is true
		if (bUseCharacterWalkingSpeed && CharacterOwner)
		{
			//set the lag min velocity to the character's walking speed
			LagMinVelocity = CharacterOwner->GetCharacterMovement()->MaxWalkSpeed;
		}
		else
		{
			//set the lag min velocity to the CameraLagMinVelocity
			LagMinVelocity = CameraLagMinVelocity;
		}

		//setup the lerp alpha
		float LerpAlpha = FMath::GetMappedRangeValueClamped(FVector2D(LagMinVelocity, CameraLagMaxVelocity), FVector2D(0.0f, 1.0f), OwnerSpeed);

		//check if the lerp alpha has changed too much since the last frame
		if (FMath::Abs(OldLerpAlpha - LerpAlpha) > CustomCameraLagSpeed * DeltaTime)
		{
			//set the lerp alpha to a value between the old lerp alpha and the new lerp alpha
			LerpAlpha = FMath::Lerp(OldLerpAlpha, LerpAlpha, CustomCameraLagSpeed * DeltaTime);
		}

		//lerp the camera lag distance between the min and max velocity
		float LagDistance = FMath::Lerp(CameraLagMinDistance, CustomCameraLagMaxDistance, LerpAlpha);

		//set the target arm length to the lag distance
		this->TargetArmLength = LagDistance;

		//set the old lerp alpha to the new lerp alpha
		OldLerpAlpha = LerpAlpha;
	}

	//call the parent implementation
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
}
