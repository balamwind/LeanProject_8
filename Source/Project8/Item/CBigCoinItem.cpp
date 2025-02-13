// Fill out your copyright notice in the Description page of Project Settings.


#include "CBigCoinItem.h"

ACBigCoinItem::ACBigCoinItem() : ACCoinItem()
{
	PointValue = 50;
	ItemType = "Big Coin";
}

void ACBigCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	DestroyItem();
}
