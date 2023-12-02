#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_Dead UMETA(DisplayName = "Dead"),
	ECS_DoingSomething UMETA(DisplayName = "DoingSomething")
};

UENUM(BlueprintType)
enum class EGrappleState : uint8
{
	EGS_Retracted UMETA(DisplayName = "Retracted"),
	EGS_InAir UMETA(DisplayName = "InAir"),
	EGS_Attached UMETA(DisplayName = "Attached")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Idle UMETA(DisplayName = "Idle"),
	EES_Occupied UMETA(DisplayName = "Occupied"),
	EES_Patroling UMETA(DisplayName = "Patroling"),
	EES_Attacking UMETA(DisplayName = "Attacking")
};

UENUM(BlueprintType)
enum class ESGunState : uint8
{
	ESGS_Idle UMETA(DisplayName = "Idle"),
	ESGS_Chargeing UMETA(DisplayName = "Chargeing"),
	ESGS_Shooting UMETA(DisplayName = "Shooting"),
	ESGS_Cooling UMETA(DisplayName = "Cooling")
};

UENUM(BlueprintType)
enum class EBossState : uint8
{
	E_PhaseZero UMETA(DisplayName = "PhaseOne"),
	E_PhaseOne UMETA(DisplayName = "PhaseOne"),
	E_PhaseTwo UMETA(DisplayName = "PhaseTwo"),
	E_PhaseThree UMETA(DisplayName = "PhaseThree")
};