// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class ATreasure;
class UCapsuleComponent;

UCLASS()
class PROJECTSLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;

	// this is needed for the `SpawnActor` method in World. We need to specify a class for the template function (in particular a BP class and not a c++ one)... In order to do this we can use the UClass and specify it in BP_BreakableActor
	// Below we use `TSubclassOf` wrapper to specify which class we want! This restricts it to ATreasure class or anything derived from it! More effective than UClass*
	UPROPERTY(EditAnywhere)
	TSubclassOf<ATreasure> TreasureClass;  

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> Capsule;

};
