// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"    // OK to be included as it's a USTRUCT and has a small memory footprint
#include "Bird.generated.h"

class UCapsuleComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;


UCLASS()
class PROJECTSLASH_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* BirdMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction = nullptr;

private:

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule = nullptr;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera = nullptr;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom = nullptr;
};
