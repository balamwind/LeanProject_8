// Fill out your copyright notice in the Description page of Project Settings.


#include "CCoinItem.h"

#include "Project8/CGameState.h"

ACCoinItem::ACCoinItem() : ACBaseItem()
{
	PointValue = 0;
	ItemType = "Default Coin";
}

void ACCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
			if(ACGameState* GameState = World->GetGameState<ACGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		
		DestroyItem();
	}
}
