// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterStates.h"
#include "SlashCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class UInputMappingContext;
class UInputAction;
class AItem;
class UAnimMontage;
class UPlayerOverlay;


UCLASS()
class PROJECTSLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ASlashCharacter();

	/*
	* Callbacks for input
	*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	void EKeyPressed();
	void EquipTwoHandedWeapon(ATwoWeapons* OverlappingWeapons);
	void EquipOneHandedWeapon(AWeapon* OverlappingWeapon);
	virtual void Attack() override;
	void Taunt();
	void Kick();
	void Dodge();
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void SetHUDHealth();
	
	//void Dash();

	/** Play Montage Functions */

	
	virtual void EndAttackMontage() override;

	UFUNCTION(BlueprintCallable)
	void EndTauntMontage();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();
	
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	void PlayEquipMontage(const FName& SectionName);
	void PlayTauntMontage();

	bool CanArm();
	bool CanDisarm();
	virtual bool CanAttack() override;

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }  // good practice to inline getters and setters
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

protected:

	virtual void BeginPlay() override;
	void InitializePlayerOverlay();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	virtual void Die() override;

private:

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera = nullptr;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* EchoMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EKeyAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* TauntAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* KickAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DodgeAction = nullptr;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* TauntMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage = nullptr;

	UPlayerOverlay* PlayerOverlay = nullptr;

};