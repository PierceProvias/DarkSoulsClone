// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interfaces/HitInterface.h"
#include "CharacterStates.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class ATwoWeapons;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class PROJECTSLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }

protected:

	virtual void BeginPlay() override;
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<ATwoWeapons> TwoWeapons;


	virtual void Attack();

	virtual void Die();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attribute;

	/**
	* Play Montage Functions
	*/

	void PlayHitReactMontage(const FName& SectionName);
	virtual void PlayDodgeMontage();

	virtual bool CanAttack();

	UFUNCTION(BlueprintCallable)
	virtual void EndAttackMontage();

	void DirectionalHitReact(const FVector& ImpactPoint);

	bool IsAlive();

	virtual void HandleDamage(float DamageAmount);
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);  // avoids making copies -> const ref

	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();

	void StopAttackMontage();

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EDeathPose> DeathPose;

private:

	/** Animation Montages */
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = "Montages")
	float AnimationSpeed = 1.f;

};
