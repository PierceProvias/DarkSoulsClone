// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterStates.h"
#include "SlashAnimInstance.generated.h"

class ASlashCharacter;
class UCharacterMovementComponent;

UCLASS()
class PROJECTSLASH_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	ASlashCharacter* Character = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* CharacterMovement = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bInAir;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	TEnumAsByte<EDeathPose> DeathPose;


	
};
