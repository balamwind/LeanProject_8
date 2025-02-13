// Fill out your copyright notice in the Description page of Project Settings.


#include "CSpawnVolume.h"

#include "Components/BoxComponent.h"
#include "Project8/CItemSpawnRow.h"

// Sets default values
ACSpawnVolume::ACSpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>("SpawningBox");
	SpawningBox->SetupAttachment(Scene);
	
	ItemDataTable = nullptr;
}

AActor* ACSpawnVolume::SpawnRandomItem()
{
	if (FCItemSpawnRow* SelectedRow = GetRandomItem())
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
			return SpawnItem(ActualClass);

	return nullptr;
}

FCItemSpawnRow* ACSpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;

	TArray<FCItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = .0f;
	for (const FCItemSpawnRow* Row : AllRows)
		if (Row)
			TotalChance += Row->SpawnChance;

	const float RandValue = FMath::FRandRange(.0f, TotalChance);
	float AccumulateChance = .0f;

	for (FCItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
			return Row;
	}

	return nullptr;
}

FVector ACSpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
	                            FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
	                            FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z));
}

AActor* ACSpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ItemClass, GetRandomPointInVolume(), FRotator::ZeroRotator);
	return SpawnedActor;
}
