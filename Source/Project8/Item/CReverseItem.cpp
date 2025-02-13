// Fill out your copyright notice in the Description page of Project Settings.


#include "CReverseItem.h"
#include "Project8/Character/CCharacter.h"


ACReverseItem::ACReverseItem()
{
	ItemType = "Reverse Item";
}

void ACReverseItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ACCharacter* player = Cast<ACCharacter>(Activator))
			player->AddReverseDebuff(15);
			
		DestroyItem();
	}
}
