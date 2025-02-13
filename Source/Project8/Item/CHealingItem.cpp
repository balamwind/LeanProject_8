// Fill out your copyright notice in the Description page of Project Settings.


#include "CHealingItem.h"

#include "Project8/Character/CCharacter.h"

ACHealingItem::ACHealingItem()
{
	HealAmount = 20.0f;
	ItemType = "Healing";
}

void ACHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ACCharacter* player = Cast<ACCharacter>(Activator))
			player->AddHealth(HealAmount);
			
		DestroyItem();
	}
}
