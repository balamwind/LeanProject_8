// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBaseItem.h"
#include "CSlowingItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT8_API ACSlowingItem : public ACBaseItem
{
	GENERATED_BODY()
	
public:
	ACSlowingItem();

	virtual void ActivateItem(AActor* Activator) override;
};
