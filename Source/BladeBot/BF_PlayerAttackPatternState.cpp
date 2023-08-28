// Fill out your copyright notice in the Description page of Project Settings.


#include "BF_PlayerAttackPatternState.h"

uint8 UBF_PlayerAttackPatternState::GetAttackPatternState()
{
	return AttackPatternIndex;
}

void UBF_PlayerAttackPatternState::SetAttackPatternState(uint8 NewAttackPatternState)
{
	AttackPatternIndex = NewAttackPatternState;
}

void UBF_PlayerAttackPatternState::IncrementAttackPatternState()
{
	AttackPatternIndex = ((uint8)AttackPatternIndex + 1);
}

void UBF_PlayerAttackPatternState::ResetAttackPatternState()
{
	AttackPatternIndex = 0;
}