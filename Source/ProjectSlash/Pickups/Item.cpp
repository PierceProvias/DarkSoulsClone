// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/SphereComponent.h"
#include "ProjectSlash/Characters/SlashCharacter.h"

AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	SphereCollider->SetupAttachment(GetRootComponent());

	SphereCollider->bHiddenInGame = true;
	SphereCollider->SetGenerateOverlapEvents(true);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASlashCharacter* Character = Cast<ASlashCharacter>(OtherActor))
	{
		Character->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ASlashCharacter* Character = Cast<ASlashCharacter>(OtherActor))
	{
		Character->SetOverlappingItem(nullptr);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
		AddActorWorldRotation(FRotator(0.f, 1.f, 0.f));
	}
}

float AItem::TransformedSin()
{
	return FMath::Sin(TimeConstant * RunningTime) * Amplitude;
}

float AItem::TransformedCos()
{
	return FMath::Cos(TimeConstant * RunningTime) * Amplitude;
}

