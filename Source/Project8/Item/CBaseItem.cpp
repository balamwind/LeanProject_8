// Fill out your copyright notice in the Description page of Project Settings.


#include "CBaseItem.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACBaseItem::ACBaseItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Collision);
}

// Called when the game starts or when spawned
void ACBaseItem::BeginPlay()
{
	Super::BeginPlay();

	//이벤트 바인딩
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACBaseItem::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ACBaseItem::OnItemEndOverlap);
}

// Called every frame
void ACBaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACBaseItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                               int32 OtherBodyIndex, bool bFormSweep, const FHitResult& SweepResult)
{	
	if (OtherActor && OtherActor->ActorHasTag("Player"))
		ActivateItem(OtherActor);
}

void ACBaseItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                  int32 OtherBodyIndex)
{
}

void ACBaseItem::ActivateItem(AActor* Activator)
{
	UParticleSystemComponent* Particle = nullptr;
	if (PickupParticle)
		Particle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupParticle, GetActorLocation(), GetActorRotation(), true);

	if (PickupSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(DestroyParticleTimerHandle, [Particle]()
		{
			Particle->DestroyComponent();
		}, 2, false);
			
	}
}

FName ACBaseItem::GetItemType() const
{
	return ItemType;
}

void ACBaseItem::DestroyItem()
{
	Destroy();
}
