// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CCoinItem.h"
#include "CBigCoinItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT8_API ACBigCoinItem : public ACCoinItem
{
	GENERATED_BODY()

public:
	ACBigCoinItem();

	virtual void ActivateItem(AActor* Activator) override;
};
