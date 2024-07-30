// Fill out your copyright notice in the Description page of Project Settings.

#include "SlashCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Controller.h"
#include "GroomComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "../Pickups/Item.h"
#include "../Pickups/Weapons/Weapon.h"
#include "../Pickups/Weapons/TwoWeapons.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../HUD/PlayerHUD.h"
#include "../HUD/PlayerOverlay.h"
#include "../Components/AttributeComponent.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	Camera->SetupAttachment(CameraBoom);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(EchoMappingContext, 0);
		}
	}
	Tags.Add(FName("EngageableTarget")); // We set a tag on the player and it's used for the enemy to chase the player (cheaper than casting)

	InitializePlayerOverlay();
}

void ASlashCharacter::InitializePlayerOverlay()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD()))
		{
			PlayerOverlay = PlayerHUD->GetPlayerOverlay();
			if (PlayerOverlay && Attribute)
			{
				PlayerOverlay->SetHealthBarPercent(Attribute->GetHealthPercent());
				PlayerOverlay->SetStaminaBarPercent(1.f);
				PlayerOverlay->SetGoldCount(0);
				PlayerOverlay->SetSoulsCount(0);
			}
		}
	}
}

void ASlashCharacter::EndAttackMontage()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::EndTauntMontage()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("Sword_Sheathed"));
	}
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("Weapon Right"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		   CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanDisarm()
{
	return (ActionState == EActionState::EAS_Unoccupied && 
		   CharacterState != ECharacterState::ECS_Unequipped);
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		   CharacterState == ECharacterState::ECS_Unequipped &&
		   EquipMontage && EquippedWeapon;
}

void ASlashCharacter::PlayTauntMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if ((AnimInstance) && (TauntMontage))
	{
		AnimInstance->Montage_Play(TauntMontage, 1.0f);
		const int32 Selection = FMath::RandRange(0, 2);  // Random generator for attack montage section
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("GolfDrive");
			break;

		case 1:
			SectionName = FName("Loser");
			break;

		case 2:
			SectionName = FName("Gervontay");
			break;

		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, TauntMontage);
	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::Attack()
{
	Super::Attack();
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::Taunt()
{
	if (CharacterState == ECharacterState::ECS_Unequipped && ActionState == EActionState::EAS_Unoccupied)
	{
		PlayTauntMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::Dodge()
{
	PlayDodgeMontage();
}

void ASlashCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit(ImpactPoint, Hitter);  // From ABaseCharacter
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	if (Attribute && Attribute->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::SetHUDHealth()
{
	if (PlayerOverlay && Attribute)
	{
		PlayerOverlay->SetHealthBarPercent(Attribute->GetHealthPercent());
	}
}

/*

void ASlashCharacter::Dash()
{
	const FVector CurrentLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector().GetSafeNormal();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FHitResult BoxHit;

	UKismetSystemLibrary::LineTraceSingle(
		this, 
		CurrentLocation, 
		CurrentLocation + (100.f * ForwardVector), 
		ETraceTypeQuery::TraceTypeQuery2, 
		false, 
		ActorsToIgnore, 
		EDrawDebugTrace::ForDuration, 
		BoxHit, 
		true
	);	
}
*/

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{

	if (ActionState != EActionState::EAS_Unoccupied) return;

	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (GetController() != nullptr)
	{
		// find out which way is forward
	
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASlashCharacter::Jump()
{
	if (ActionState == EActionState::EAS_Unoccupied)
	{
		Super::Jump();	
	}
}

void ASlashCharacter::EKeyPressed()
{
	if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
	{
		EquipOneHandedWeapon(OverlappingWeapon);
	}
	else if (ATwoWeapons* OverlappingWeapons = Cast<ATwoWeapons>(OverlappingItem))
	{
		EquipTwoHandedWeapon(OverlappingWeapons);
	}
	else
	{
		if (CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;	
		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::EquipTwoHandedWeapon(ATwoWeapons* OverlappingWeapons)
{
	OverlappingWeapons->Equip(GetMesh(), FName("Weapon Right"), FName("Weapon Left"));
	CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
	OverlappingItem = nullptr;
	TwoWeapons = OverlappingWeapons;
}

void ASlashCharacter::EquipOneHandedWeapon(AWeapon* OverlappingWeapon)
{
	OverlappingWeapon->Equip(GetMesh(), FName("Weapon Right"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = OverlappingWeapon;
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(TauntAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Taunt);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
		//EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dash);
	}
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Die()
{
	Super::Die();
	ActionState = EActionState::EAS_Dead;	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


