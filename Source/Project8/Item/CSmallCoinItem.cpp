// Fill out your copyright notice in the Description page of Project Settings.


#include "CSmallCoinItem.h"

ACSmallCoinItem::ACSmallCoinItem() : ACCoinItem()
{
	PointValue = 10;
	ItemType = "Small Coin";
}

void ACSmallCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	DestroyItem();
}
