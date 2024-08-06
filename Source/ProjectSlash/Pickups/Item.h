// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class PROJECTSLASH_API AItem : public AActor
{
	GENERATED_BODY()
	

public:	
	
	AItem();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();
	
protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleInstanceOnly, Category = "Collision")
	TObjectPtr<USphereComponent> SphereCollider;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sinusoids", meta = (AllowPrivateAccess = "true"))
	float Amplitude = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sinusoids", meta = (AllowPrivateAccess = "true"))
	float TimeConstant = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sinusoids", meta = (AllowPrivateAccess = "true"))
	float RunningTime;

public:

	FORCEINLINE USphereComponent* GetSphereCollider() const { return SphereCollider; }
	
};
