// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBaseItem.h"
#include "CReverseItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT8_API ACReverseItem : public ACBaseItem
{
	GENERATED_BODY()

	
public:
	ACReverseItem();

	virtual void ActivateItem(AActor* Activator) override;
};
