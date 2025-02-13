// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSpawnVolume.generated.h"

struct FCItemSpawnRow;
class UBoxComponent;

UCLASS()
class PROJECT8_API ACSpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:
	ACSpawnVolume();

	UFUNCTION(BlueprintCallable, Category="Spawning")
	AActor* SpawnRandomItem();
	
	FCItemSpawnRow* GetRandomItem() const;
	FVector GetRandomPointInVolume() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning")
	UBoxComponent* SpawningBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawning")
	UDataTable* ItemDataTable;	
};
