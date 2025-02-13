// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CBaseItem.h"
#include "CHealingItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT8_API ACHealingItem : public ACBaseItem
{
	GENERATED_BODY()

public:
	ACHealingItem();

	virtual void ActivateItem(AActor* Activator) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 HealAmount;
};
