// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTSLASH_API IHitInterface
{
	GENERATED_BODY()

	
public:
	
	// const reference so it doesnt make a copy (rare to pass by value for FVector)
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) = 0; // virtual pure function
};
