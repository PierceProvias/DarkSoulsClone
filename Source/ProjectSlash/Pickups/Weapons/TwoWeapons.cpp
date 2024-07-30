// Fill out your copyright notice in the Description page of Project Settings.


#include "TwoWeapons.h"
#include "ProjectSlash/Characters/SlashCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectSlash/Interfaces/HitInterface.h"

ATwoWeapons::ATwoWeapons()
{
	SecondMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Second Mesh"));
	SecondMesh->SetupAttachment(GetRootComponent());

	RightWeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightBoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Right Box Trace Start"));
	RightBoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Right Box Trace End"));

	LeftWeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Weapon Box"));
	LeftBoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Left Box Trace Start"));
	LeftBoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Left Box Trace End"));

	RightWeaponBox->SetupAttachment(GetRootComponent());
	RightBoxTraceStart->SetupAttachment(GetRootComponent());
	RightBoxTraceEnd->SetupAttachment(GetRootComponent());

	LeftWeaponBox->SetupAttachment(GetRootComponent());
	LeftBoxTraceStart->SetupAttachment(GetRootComponent());
	LeftBoxTraceEnd->SetupAttachment(GetRootComponent());

	RightWeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);  // Checks all boxes in collision responses in BP_Weapon to Overlap
	RightWeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);  // Ignores specific channel (the pawn channel)

	LeftWeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);  
	LeftWeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);  
}

void ATwoWeapons::Equip(USceneComponent* InParent, FName InSocketName, FName SecondInSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	Mesh->AttachToComponent(InParent, TransformRules, InSocketName);
	SecondMesh->AttachToComponent(InParent, TransformRules, SecondInSocketName);
	ItemState = EItemState::EIS_Equipped;
}

void ATwoWeapons::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ATwoWeapons::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void ATwoWeapons::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector RightStart = RightBoxTraceStart->GetComponentLocation();
	const FVector RightEnd = RightBoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		RightStart,
		RightEnd,
		FVector(5.f, 5.f, 5.f),
		RightBoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		BoxHit,
		true);

	if (BoxHit.GetActor())
	{
		//IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor()))
		{
			HitInterface->GetHit(BoxHit.ImpactPoint, GetOwner());
		}
	}
}
