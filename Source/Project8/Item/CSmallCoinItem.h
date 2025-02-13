// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CCoinItem.h"
#include "CSmallCoinItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT8_API ACSmallCoinItem : public ACCoinItem
{
	GENERATED_BODY()
	
public:
	ACSmallCoinItem();

	virtual void ActivateItem(AActor* Activator) override;
};
