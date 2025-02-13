// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameInstance.h"

UCGameInstance::UCGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UCGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Total Scroe Updated : %d"), TotalScore);
}
