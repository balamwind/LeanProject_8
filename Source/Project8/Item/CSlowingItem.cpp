
#include "CSlowingItem.h"
#include "Project8/Character/CCharacter.h"


ACSlowingItem::ACSlowingItem()
{
	ItemType = "Slowing Item";
}

void ACSlowingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ACCharacter* player = Cast<ACCharacter>(Activator))
			player->AddSlowDebuff(0.5,  10);
			
		DestroyItem();
	}
}
