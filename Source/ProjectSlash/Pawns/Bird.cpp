// Fill out your copyright notice in the Description page of Project Settings.


#include "Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(12.f);
	SetRootComponent(Capsule);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	Camera->SetupAttachment(CameraBoom);
	
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SkeletalMesh->SetupAttachment(Capsule);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom Component"));
	CameraBoom->SetupAttachment(Capsule);
	CameraBoom->TargetArmLength = 300.f;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ABird::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(BirdMappingContext, 0);
		}
	}
	
}

void ABird::Move(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();

	if (Controller && (DirectionValue != 0.f))
	{
		FVector ForwardVector = GetActorForwardVector();
		AddMovementInput(ForwardVector, DirectionValue);
	}
}

void ABird::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}
}

