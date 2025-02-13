// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBaseItem.h"
#include "CCoinItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT8_API ACCoinItem : public ACBaseItem
{
	GENERATED_BODY()

public:
	ACCoinItem();

	virtual void ActivateItem(AActor* Activator) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	int32 PointValue;
};
