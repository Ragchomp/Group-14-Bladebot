// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "AsyncRootMovement.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementEvent);

USTRUCT(BlueprintType)
struct FAsyncRootMovementParams
{
	GENERATED_BODY()

    //constructor
	explicit FAsyncRootMovementParams(
        FVector InWorldDirection = FVector::ZeroVector,
        int InPriority = 1,
		float InStrength = 0.0f,
		float InDuration = 0.0f,
		bool InbIsAdditive = false,
		UCurveFloat* InStrengthOverTime = nullptr,
		ERootMotionFinishVelocityMode InVelocityOnFinishMode = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
		FVector InSetVelocityOnFinish = FVector::ZeroVector,
		float InClampVelocityOnFinish = 0.0f,
		bool InbEnableGravity = false,
		int InRootMotionSourceID = 0)
		: WorldDirection(InWorldDirection)
		, Priority(InPriority)
		, Strength(InStrength)
		, Duration(InDuration)
		, bIsAdditive(InbIsAdditive)
		, StrengthOverTime(InStrengthOverTime)
		, VelocityOnFinishMode(InVelocityOnFinishMode)
		, SetVelocityOnFinish(InSetVelocityOnFinish)
		, ClampVelocityOnFinish(InClampVelocityOnFinish)
		, bEnableGravity(InbEnableGravity)
		, RootMotionSourceID(InRootMotionSourceID)
	{};

    /* Timer handle for OnComplete management */
    FTimerHandle OngoingDelay;

    /* World Direction of movement */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    FVector WorldDirection = FVector::ZeroVector;

    /* the priority of this movement */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    int Priority = 1;

    /* Strength of movement */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    float Strength = 0.0f;

    /* Duration of movement */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    float Duration = 0.0f;

    /* If the movement is additive or not */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    bool bIsAdditive = false;

    /* Curve with strenght of movement over time */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    UCurveFloat* StrengthOverTime = nullptr;

    /* Velocity Finish Mode of movement */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    ERootMotionFinishVelocityMode VelocityOnFinishMode;

    /* Velocity to set after movement finishes in SetVelocity mode; Ignored otherwise */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    FVector SetVelocityOnFinish = FVector::ZeroVector;

    /* Clamp value to use after movement finishes in clamp mode; Ignored otherwise */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    float ClampVelocityOnFinish = 0.0f;

    /* Weather or not gravity should be enabled for movement */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    bool bEnableGravity = false;

    /* Place to save the active root motion source ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMovement")
    int RootMotionSourceID = 0;

    /* A delegate called when the async action completes. */
    UPROPERTY(BlueprintAssignable)
	FMovementEvent OnComplete;

    /* A delegate called when the async action fails. */
    UPROPERTY(BlueprintAssignable)
        FMovementEvent OnFail;
};

/**
 * Enables root based movement with callbacks
 */
UCLASS()
class UAsyncRootMovement : public UCancellableAsyncAction
{
    GENERATED_BODY()

    /* The context world of this action. */
    TWeakObjectPtr<UWorld> ContextWorld = nullptr;

    /* Timer handle for OnComplete management */
    FTimerHandle OngoingDelay;

    /* Character movement component to apply root movement to */
    UCharacterMovementComponent* CharacterMovement;

    /* World Direction of movement */
    FVector WorldDirection;

    /* Strength of movement */
    float Strength;

    /* Duration of movement */
    float Duration;

    /* If the movement is additive or not */
    bool bIsAdditive;

    /* Curve with strenght of movement over time */
    UCurveFloat* StrengthOverTime;

    /* Velocity Finish Mode of movement */
    ERootMotionFinishVelocityMode VelocityOnFinishMode;

    /* Velocity to set after movement finishes in SetVelocity mode; Ignored otherwise */
    FVector SetVelocityOnFinish;

    /* Clamp value to use after movement finishes in clamp mode; Ignored otherwise */
    float ClampVelocityOnFinish;

    /* Weather or not gravity should be enabled for movement */
    bool bEnableGravity;

    /* Place to save the active root motion source ID */
    int RootMotionSourceID;

    /* A delegate called when the async action completes. */
    UPROPERTY(BlueprintAssignable)
        FMovementEvent OnComplete;

    /* A delegate called when the async action fails. */
    UPROPERTY(BlueprintAssignable)
        FMovementEvent OnFail;

    //storage for the root motion force
    TSharedPtr<FRootMotionSource_ConstantForce> ConstantForce;

public:
    /**
     * Apply root motion movement to passed Character Movement Component
     */
    UFUNCTION(BlueprintCallable, DisplayName = "Apply Root Motion Constant Force with Callbacks", Category = "RootMovement", meta = (WorldContext = "WorldContext", BlueprintInternalUseOnly = "true", Keywords = "RootMovement RootMotion Root Motion Movement"))
    static UAsyncRootMovement* AsyncRootMovement(
        const UObject* WorldContext,
        UCharacterMovementComponent* CharacterMovement,
        FVector WorldDirection,
        float Strength,
        float Duration,
        bool bIsAdditive,
        UCurveFloat* StrengthOverTime,
        ERootMotionFinishVelocityMode VelocityOnFinishMode,
        FVector SetVelocityOnFinish,
        float ClampVelocityOnFinish,
        bool bEnableGravity
    );

    // Override functions from parent
    virtual void Activate() override;
    virtual void Cancel() override;

    virtual UWorld* GetWorld() const override
    {
        return ContextWorld.IsValid() ? ContextWorld.Get() : nullptr;
    }

    static UAsyncRootMovement* AsyncRootMovement(const UObject* WorldContext, UCharacterMovementComponent* CharacterMovement, FAsyncRootMovementParams Params);
};
