// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashAnimInstance.h"
#include "SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ASlashCharacter>(TryGetPawnOwner());
	if (Character)
	{
		CharacterMovement = Character->GetCharacterMovement();
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (CharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);
		CharacterState = Character->GetCharacterState();
		ActionState = Character->GetActionState();
		DeathPose = Character->GetDeathPose();
	}
}
