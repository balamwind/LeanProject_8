// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CItemInterface.h"
#include "GameFramework/Actor.h"
#include "CBaseItem.generated.h"

class USphereComponent;

UCLASS()
class PROJECT8_API ACBaseItem : public AActor, public ICItemInterface
{
	GENERATED_BODY()

public:
	ACBaseItem();

	virtual void Tick(float DeltaTime) override;
	virtual void DestroyItem();
	
protected:
	virtual void BeginPlay() override;
	virtual void ActivateItem(AActor* Activator) override;
	virtual FName GetItemType() const override;
	virtual void OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
							   int32 OtherBodyIndex, bool bFormSweep, const FHitResult& SweepResult) override;
	virtual void OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								  int32 OtherBodyIndex) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item|Component")
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item|Component")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|Effects")
	UParticleSystem* PickupParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|Effects")
	USoundBase* PickupSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FName ItemType;

};
