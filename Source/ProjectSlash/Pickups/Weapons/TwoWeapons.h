// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Item.h"
#include "TwoWeapons.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTSLASH_API ATwoWeapons : public AItem
{
	GENERATED_BODY()


public:

	ATwoWeapons();
	void Equip(USceneComponent* InParent, FName InSocketName, FName SecondInSocketName);
	FORCEINLINE UBoxComponent* GetRightWeaponBox() const { return RightWeaponBox; }
	FORCEINLINE UBoxComponent* GetLeftWeaponBox() const { return LeftWeaponBox; }

protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	
	UPROPERTY(VisibleInstanceOnly, Category = "Second Mesh")
	UStaticMeshComponent* SecondMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* RightWeaponBox = nullptr;

	UPROPERTY(VisibleAnywhere)  // Typically "VisibleAnywhere" is used for USceneComponents whereas variables will have "EditAnywhere"
	USceneComponent* RightBoxTraceStart = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* RightBoxTraceEnd = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* LeftWeaponBox = nullptr;

	UPROPERTY(VisibleAnywhere) 
	USceneComponent* LeftBoxTraceStart = nullptr;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* LeftBoxTraceEnd = nullptr;
};
