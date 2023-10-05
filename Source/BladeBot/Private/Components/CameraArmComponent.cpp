#include "Components/CameraArmComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCameraArmComponent::UCameraArmComponent(const FObjectInitializer& ObjectInitializer): OldTargetArmLength(0)
{
	//set the default values
	CameraLagMaxDistance = 800;
}

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
		//the minimum speed to use for the camera lag
		float MinSpeed;

		//check if bUseCharacterWalkingSpeed is true and if the character owner is valid
		if (bUseCharWalkSpeed && CharacterOwner)
		{
			//set the minimum speed to the character's walking speed
			MinSpeed = CharacterOwner->GetCharacterMovement()->MaxWalkSpeed;
		}
		else
		{
			//set the minimum speed to the CameraOwnerMinSpeed
			MinSpeed = CameraOwnerMinSpeed;
		}

		//the speed of movement
		float Speed;

		//check if we should ignore the z axis
		if (bIgnoreZVelocity)
		{
			//set the speed while ignoring the z axis
			Speed = FVector(GetOwner()->GetVelocity().X, GetOwner()->GetVelocity().Y, 0).Length();
		}
		else
		{
			//set the speed while not ignoring the z axis
			Speed = GetOwner()->GetVelocity().Length();
		}

		//clamp the speed
		Speed = FMath::Clamp(Speed, MinSpeed, CameraOwnerMaxSpeed);

		//the alpha value for the interpolation
		const float Alpha = FMath::Clamp(Speed / CameraOwnerMaxSpeed, 0, 1);

		//the new target arm length
		float NewTargetArmLength = Interp<float>(InterpolationType, MinArmLength, MaxArmLength, Alpha, InterpolationControl);

		//check if the old target arm length has been set
		if (OldTargetArmLength != 0)
		{
			//check if the difference between the new target arm length and the old target arm length is greater than the max lag speed
			if (FMath::Abs(NewTargetArmLength - OldTargetArmLength) > MaxLagSpeed)
			{
				//the max speed the camera can lag behind the player with delta time applied
				const float LocMaxLagSpeed = MaxLagSpeed * DeltaTime / 1000;

				//check if the new target arm length is greater than the old target arm length
				if (NewTargetArmLength > OldTargetArmLength)
				{
					//set the new target arm length to the old target arm length plus the max lag speed
					NewTargetArmLength = OldTargetArmLength + LocMaxLagSpeed;
				}
				else
				{
					//set the new target arm length to the old target arm length minus the max lag speed
					NewTargetArmLength = OldTargetArmLength - LocMaxLagSpeed;
				}
			}
		}

		//set the target arm length
		TargetArmLength = NewTargetArmLength;

		//update the old target arm length
		OldTargetArmLength = TargetArmLength;
	}

	//call the parent implementation
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
}
